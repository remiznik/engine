#include "render.h"


using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;


namespace render
{	
	RenderD12::RenderD12()
	{

	}

	bool RenderD12::initialize(HWND hMainWnd)
	{
		hMainWnd_ = hMainWnd;

#if defined(DEBUG) || defined(_DEBUG) 
		// Enable the D3D12 debug layer.
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
#endif

		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_)));

		// Try to create hardware device.
		HRESULT hardwareResult = D3D12CreateDevice(
			nullptr,             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&d3dDevice_));

		// Fallback to WARP device.
		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(dxgiFactory_->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

			ThrowIfFailed(D3D12CreateDevice(
				pWarpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&d3dDevice_)));
		}

		ThrowIfFailed(d3dDevice_->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence_)));

		RtvDescriptorSize_ = d3dDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DsvDescriptorSize_ = d3dDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		CbvSrvUavDescriptorSize_ = d3dDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render 
		// target formats, so we only need to check quality support.

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = BackBufferFormat_;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(d3dDevice_->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

		_4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
		//LogAdapters();
#endif

		CreateCommandObjects();
		CreateSwapChain();
		CreateRtvAndDsvDescriptorHeaps();

		resize();

		ThrowIfFailed(CommandList_->Reset(DirectCmdListAlloc_.Get(), nullptr));

		BuildDescriptorHeaps();
		BuildConstantBuffers();
		BuildRootSignature();
		BuildShadersAndInputLayout();
		BuildBoxGeometry();
		BuildPSO();

		ThrowIfFailed(CommandList_->Close());
		ID3D12CommandList* cmdsLists[] = { CommandList_.Get() };
		CommandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		FlushCommandQueue();

		return true;
	}

	void RenderD12::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(d3dDevice_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue_)));

		ThrowIfFailed(d3dDevice_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(DirectCmdListAlloc_.GetAddressOf())));

		ThrowIfFailed(d3dDevice_->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			DirectCmdListAlloc_.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(CommandList_.GetAddressOf())));
		  
		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		CommandList_->Close();
	}

	void RenderD12::CreateSwapChain()
	{
		// Release the previous swapchain we will be recreating.
		SwapChain_.Reset();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = ClientWidth_;
		sd.BufferDesc.Height = ClientHeight_;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = BackBufferFormat_;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = _4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;
		sd.OutputWindow = hMainWnd_;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(dxgiFactory_->CreateSwapChain(
			CommandQueue_.Get(),
			&sd,
			SwapChain_.GetAddressOf()));
	}

	void RenderD12::CreateRtvAndDsvDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(d3dDevice_->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(RtvHeap_.GetAddressOf())));


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(d3dDevice_->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(DsvHeap_.GetAddressOf())));
	}

	void RenderD12::resize()
	{
		assert(d3dDevice_);
		assert(SwapChain_);
		assert(DirectCmdListAlloc_);

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowIfFailed(CommandList_->Reset(DirectCmdListAlloc_.Get(), nullptr));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < SwapChainBufferCount; ++i)
			SwapChainBuffer_[i].Reset();
		DepthStencilBuffer_.Reset();

		// Resize the swap chain.
		ThrowIfFailed(SwapChain_->ResizeBuffers(
			SwapChainBufferCount,
			ClientWidth_, ClientHeight_,
			BackBufferFormat_,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		CurrBackBuffer_ = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap_->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			ThrowIfFailed(SwapChain_->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer_[i])));
			d3dDevice_->CreateRenderTargetView(SwapChainBuffer_[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, RtvDescriptorSize_);
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = ClientWidth_;
		depthStencilDesc.Height = ClientHeight_;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = _4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DepthStencilFormat_;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		ThrowIfFailed(d3dDevice_->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(DepthStencilBuffer_.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DepthStencilFormat_;
		dsvDesc.Texture2D.MipSlice = 0;
		d3dDevice_->CreateDepthStencilView(DepthStencilBuffer_.Get(), &dsvDesc, DepthStencilView());

		// Transition the resource from its initial state to be used as a depth buffer.
		CommandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer_.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Execute the resize commands.
		ThrowIfFailed(CommandList_->Close());
		ID3D12CommandList* cmdsLists[] = { CommandList_.Get() };
		CommandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		ScreenViewport_.TopLeftX = 0;
		ScreenViewport_.TopLeftY = 0;
		ScreenViewport_.Width = static_cast<float>(ClientWidth_);
		ScreenViewport_.Height = static_cast<float>(ClientHeight_);
		ScreenViewport_.MinDepth = 0.0f;
		ScreenViewport_.MaxDepth = 1.0f;

		ScissorRect_ = { 0, 0, ClientWidth_, ClientHeight_ };

		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, (static_cast<float>(ClientWidth_) / ClientHeight_) , 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, P);
	}

	void RenderD12::FlushCommandQueue()
	{
		// Advance the fence value to mark commands up to this fence point.
		CurrentFence_++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		ThrowIfFailed(CommandQueue_->Signal(fence_.Get(), CurrentFence_));

		// Wait until the GPU has completed commands up to this fence point.
		if (fence_->GetCompletedValue() < CurrentFence_)
		{
			HANDLE eventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(fence_->SetEventOnCompletion(CurrentFence_, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderD12::DepthStencilView() const
	{
		return DsvHeap_->GetCPUDescriptorHandleForHeapStart();
	}

	void RenderD12::draw()
	{
		// Convert Spherical to Cartesian coordinates.
		float x = mRadius * sinf(mPhi) * cosf(mTheta);
		float z = mRadius * sinf(mPhi) * sinf(mTheta);
		float y = mRadius * cosf(mPhi);

		// Build the view matrix.
		XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&mView, view);

		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX proj = XMLoadFloat4x4(&mProj);
		XMMATRIX worldViewProj = world * view * proj;

		// Update the constant buffer with the latest worldViewProj matrix.
		ObjectConstants objConstants;
		XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
		ObjectCB_->CopyData(0, objConstants);

		ThrowIfFailed(DirectCmdListAlloc_->Reset());

		ThrowIfFailed(CommandList_->Reset(DirectCmdListAlloc_.Get(), PSO_.Get()));

		CommandList_->RSSetViewports(1, &ScreenViewport_);
		CommandList_->RSSetScissorRects(1, &ScissorRect_);

		CommandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CommandList_->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
		CommandList_->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		CommandList_->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { CbvHeap_.Get() };
		CommandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		CommandList_->SetGraphicsRootSignature(RootSignature_.Get());

		CommandList_->IASetVertexBuffers(0, 1, &BoxGeo_->VertexBufferView());		
		CommandList_->IASetIndexBuffer(&BoxGeo_->IndexBufferView());
		CommandList_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CommandList_->SetGraphicsRootDescriptorTable(0, CbvHeap_->GetGPUDescriptorHandleForHeapStart());

		CommandList_->DrawIndexedInstanced(
			BoxGeo_->DrawArgs["box"].IndexCount,
			1, 0, 0, 0);


		//CommandList_->DrawIndexedInstanced(BoxGeo_->DrawArgs["box"].IndexCount, 1, 0, 0, 0);
		//mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount, 1, 0, 1, 0);

		CommandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


		// Done recording commands.
		ThrowIfFailed(CommandList_->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { CommandList_.Get() };
		CommandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		ThrowIfFailed(SwapChain_->Present(0, 0));
		CurrBackBuffer_ = (CurrBackBuffer_ + 1) % SwapChainBufferCount;

		FlushCommandQueue();
	}

	ID3D12Resource* RenderD12::CurrentBackBuffer() const
	{
		return SwapChainBuffer_[CurrBackBuffer_].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderD12::CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			RtvHeap_->GetCPUDescriptorHandleForHeapStart(),
			CurrBackBuffer_,
			RtvDescriptorSize_);
	}


	void RenderD12::BuildDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(d3dDevice_->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&CbvHeap_)));
	}

	void RenderD12::BuildConstantBuffers()
	{
		ObjectCB_ = std::make_unique<UploadBuffer<ObjectConstants>>(d3dDevice_.Get(), 1, true);

		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ObjectCB_->Resource()->GetGPUVirtualAddress();
		// Offset to the ith object constant buffer in the buffer.
		int boxCBufIndex = 0;
		cbAddress += boxCBufIndex * objCBByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		d3dDevice_->CreateConstantBufferView(
			&cbvDesc,
			CbvHeap_->GetCPUDescriptorHandleForHeapStart());
	}

	void RenderD12::BuildRootSignature()
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[1];
		CD3DX12_DESCRIPTOR_RANGE cbvTable;
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(d3dDevice_->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&RootSignature_)));
	}

	void RenderD12::BuildShadersAndInputLayout()
	{
		HRESULT hr = S_OK;

		vsByteCode_ = d3dUtil::CompileShader(L"..\\res\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
		psByteCode_ = d3dUtil::CompileShader(L"..\\res\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

		InputLayout_ =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
	}


	void RenderD12::BuildPSO()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		psoDesc.InputLayout = { InputLayout_.data(), (UINT)InputLayout_.size() };
		psoDesc.pRootSignature = RootSignature_.Get();
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(vsByteCode_->GetBufferPointer()), vsByteCode_->GetBufferSize()
		};
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(psByteCode_->GetBufferPointer()), psByteCode_->GetBufferSize()
		};
		CD3DX12_RASTERIZER_DESC rsDec(D3D12_DEFAULT);
		//rsDec.FillMode = D3D12_FILL_MODE_WIREFRAME;
		//rsDec.CullMode = D3D12_CULL_MODE_FRONT;

		psoDesc.RasterizerState = rsDec;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = BackBufferFormat_;
		psoDesc.SampleDesc.Count = _4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
		psoDesc.DSVFormat = DepthStencilFormat_;

		ThrowIfFailed(d3dDevice_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO_)));
	};

	void RenderD12::BuildBoxGeometry()
	{
		std::array<Vertex, 8> vertices =
		{
			Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
			Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
		};

		std::array<std::uint16_t, 12 * 3> indices =
		{
			// front face 
			0, 1, 2,
			0, 2, 3,
			// back face
			4, 6, 5,
			4, 7, 6,
			// left face 
			4, 5, 1,
			4, 1, 0,
			// right face 
			3, 2, 6,
			3, 6, 7,
			// top face 
			1, 5, 6,
			1, 6, 2,
			// bottom face 
			4, 0, 3,
			4, 3, 7
		};

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		BoxGeo_ = std::make_unique<MeshGeometry>();
		BoxGeo_->Name = "boxGeo";

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &BoxGeo_->VertexBufferCPU));
		CopyMemory(BoxGeo_->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &BoxGeo_->IndexBufferCPU));
		CopyMemory(BoxGeo_->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		BoxGeo_->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(d3dDevice_.Get(),
			CommandList_.Get(), vertices.data(), vbByteSize, BoxGeo_->VertexBufferUploader);

		BoxGeo_->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(d3dDevice_.Get(),
			CommandList_.Get(), indices.data(), ibByteSize, BoxGeo_->IndexBufferUploader);

		BoxGeo_->VertexByteStride = sizeof(Vertex);
		BoxGeo_->VertexBufferByteSize = vbByteSize;
		BoxGeo_->IndexFormat = DXGI_FORMAT_R16_UINT;
		BoxGeo_->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		BoxGeo_->DrawArgs["box"] = submesh;
	}




}
