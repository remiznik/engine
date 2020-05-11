#include "render.h"


using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

const int gNumFrameResources = 3;

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
		cbvSrvUavDescriptorSize_ = d3dDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render 
		// target formats, so we only need to check quality support.

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = backBufferFormat_;
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

		// --------------------------------------------------------------------------

		ThrowIfFailed(commandList_->Reset(DirectCmdListAlloc_.Get(), nullptr));

		BuildRootSignature();
		BuildShadersAndInputLayout();
		BuildShapeGeometry();
		BuildRenderItems();
		BuildFrameResources();
		BuildDescriptorHeaps();
		BuildConstantBufferViews();
		BuildPSOs();

		
		ThrowIfFailed(commandList_->Close());
		ID3D12CommandList* cmdsLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until initialization is complete.
		FlushCommandQueue();

		return true;
	}

	void RenderD12::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(d3dDevice_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_)));

		ThrowIfFailed(d3dDevice_->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(DirectCmdListAlloc_.GetAddressOf())));

		ThrowIfFailed(d3dDevice_->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			DirectCmdListAlloc_.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(commandList_.GetAddressOf())));
		  
		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		commandList_->Close();
	}

	void RenderD12::CreateSwapChain()
	{
		// Release the previous swapchain we will be recreating.
		swapChain_.Reset();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = clientWidth_;
		sd.BufferDesc.Height = clientHeight_;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = backBufferFormat_;
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
			commandQueue_.Get(),
			&sd,
			swapChain_.GetAddressOf()));
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
		assert(swapChain_);
		assert(DirectCmdListAlloc_);

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowIfFailed(commandList_->Reset(DirectCmdListAlloc_.Get(), nullptr));

		// Release the previous resources we will be recreating.
		for (int i = 0; i < SwapChainBufferCount; ++i)
			SwapChainBuffer_[i].Reset();
		DepthStencilBuffer_.Reset();

		// Resize the swap chain.
		ThrowIfFailed(swapChain_->ResizeBuffers(
			SwapChainBufferCount,
			clientWidth_, clientHeight_,
			backBufferFormat_,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		currBackBuffer_ = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap_->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			ThrowIfFailed(swapChain_->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer_[i])));
			d3dDevice_->CreateRenderTargetView(SwapChainBuffer_[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, RtvDescriptorSize_);
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = clientWidth_;
		depthStencilDesc.Height = clientHeight_;
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
		optClear.Format = depthStencilFormat_;
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
		dsvDesc.Format = depthStencilFormat_;
		dsvDesc.Texture2D.MipSlice = 0;
		d3dDevice_->CreateDepthStencilView(DepthStencilBuffer_.Get(), &dsvDesc, DepthStencilView());

		// Transition the resource from its initial state to be used as a depth buffer.
		commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer_.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Execute the resize commands.
		ThrowIfFailed(commandList_->Close());
		ID3D12CommandList* cmdsLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		screenViewport_.TopLeftX = 0;
		screenViewport_.TopLeftY = 0;
		screenViewport_.Width = static_cast<float>(clientWidth_);
		screenViewport_.Height = static_cast<float>(clientHeight_);
		screenViewport_.MinDepth = 0.0f;
		screenViewport_.MaxDepth = 1.0f;

		scissorRect_ = { 0, 0, clientWidth_, clientHeight_ };

		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, (static_cast<float>(clientWidth_) / clientHeight_) , 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, P);
	}

	void RenderD12::FlushCommandQueue()
	{
		// Advance the fence value to mark commands up to this fence point.
		currentFence_++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		ThrowIfFailed(commandQueue_->Signal(fence_.Get(), currentFence_));

		// Wait until the GPU has completed commands up to this fence point.
		if (fence_->GetCompletedValue() < currentFence_)
		{
			HANDLE eventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(fence_->SetEventOnCompletion(currentFence_, eventHandle));

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

		UpdateObjectCBs();
		UpdateMainPassCB();		
		

		commandList_->RSSetViewports(1, &screenViewport_);
		commandList_->RSSetScissorRects(1, &scissorRect_);

		// Indicate a state transition on the resource usage.
		commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the back buffer and depth buffer.
		commandList_->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
		commandList_->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		commandList_->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap_.Get() };
		commandList_->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		commandList_->SetGraphicsRootSignature(rootSignature_.Get());

		int passCbvIndex = passCbvOffset_ + currFrameResourceIndex_;
		auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
		passCbvHandle.Offset(passCbvIndex, cbvSrvUavDescriptorSize_);
		commandList_->SetGraphicsRootDescriptorTable(1, passCbvHandle);

		DrawRenderItems(commandList_.Get(), opaqueRitems_);

		// Indicate a state transition on the resource usage.
		commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands.
		ThrowIfFailed(commandList_->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { commandList_.Get() };
		commandQueue_->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Swap the back and front buffers
		ThrowIfFailed(swapChain_->Present(0, 0));
		currBackBuffer_ = (currBackBuffer_ + 1) % SwapChainBufferCount;

		// Advance the fence value to mark commands up to this fence point.
		currFrameResource_->Fence = ++currentFence_;

		// Add an instruction to the command queue to set a new fence point. 
		// Because we are on the GPU timeline, the new fence point won't be 
		// set until the GPU finishes processing all the commands prior to this Signal().
		commandQueue_->Signal(fence_.Get(), currentFence_);
	}

	ID3D12Resource* RenderD12::CurrentBackBuffer() const
	{
		return SwapChainBuffer_[currBackBuffer_].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderD12::CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			RtvHeap_->GetCPUDescriptorHandleForHeapStart(),
			currBackBuffer_,
			RtvDescriptorSize_);
	}
	
	void RenderD12::BuildShadersAndInputLayout()
	{
		shaders_["standardVS"] = d3dUtil::CompileShader(L"..\\res\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
		shaders_["opaquePS"] = d3dUtil::CompileShader(L"..\\res\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

		inputLayout_ =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
	}

	void RenderD12::BuildRootSignature()
	{
		CD3DX12_DESCRIPTOR_RANGE cbvTable0;
		cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE cbvTable1;
		cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[2];

		// Create root CBVs.
		slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
		slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(d3dDevice_->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSignature_.GetAddressOf())));
	}

	void RenderD12::BuildShapeGeometry()
	{
		
	}

	void RenderD12::BuildRenderItems()
	{
	
	}

	void RenderD12::BuildFrameResources()
	{		
		for (int i = 0; i < gNumFrameResources; ++i)
		{
			frameResources_.push_back(std::make_unique<FrameResource>(d3dDevice_.Get(),
				1, (UINT)maxOpaqueItems_));
		}
	}

	void RenderD12::BuildDescriptorHeaps()
	{
		UINT objCount = (UINT)maxOpaqueItems_;

		// Need a CBV descriptor for each object for each frame resource,
		// +1 for the perPass CBV for each frame resource.
		UINT numDescriptors = (objCount + 1) * gNumFrameResources;

		// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
		passCbvOffset_ = objCount * gNumFrameResources;

		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = numDescriptors;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(d3dDevice_->CreateDescriptorHeap(&cbvHeapDesc,
			IID_PPV_ARGS(&cbvHeap_)));
	}

	void RenderD12::BuildConstantBufferViews()
	{
		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		UINT objCount = (UINT)maxOpaqueItems_;

		// Need a CBV descriptor for each object for each frame resource.
		for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
		{
			auto objectCB = frameResources_[frameIndex]->ObjectCB->Resource();
			for (UINT i = 0; i < objCount; ++i)
			{
				D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

				// Offset to the ith object constant buffer in the buffer.
				cbAddress += i * objCBByteSize;

				// Offset to the object cbv in the descriptor heap.
				int heapIndex = frameIndex * objCount + i;
				auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
				handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
				cbvDesc.BufferLocation = cbAddress;
				cbvDesc.SizeInBytes = objCBByteSize;

				d3dDevice_->CreateConstantBufferView(&cbvDesc, handle);
			}
		}

		UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

		// Last three descriptors are the pass CBVs for each frame resource.
		for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
		{
			auto passCB = frameResources_[frameIndex]->PassCB->Resource();
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

			// Offset to the pass cbv in the descriptor heap.
			int heapIndex = passCbvOffset_ + frameIndex;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap_->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, cbvSrvUavDescriptorSize_);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = passCBByteSize;

			d3dDevice_->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	void RenderD12::BuildPSOs()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

		//
		// PSO for opaque objects.
		//
		ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		opaquePsoDesc.InputLayout = { inputLayout_.data(), (UINT)inputLayout_.size() };
		opaquePsoDesc.pRootSignature = rootSignature_.Get();
		opaquePsoDesc.VS =
		{
			reinterpret_cast<BYTE*>(shaders_["standardVS"]->GetBufferPointer()),
			shaders_["standardVS"]->GetBufferSize()
		};
		opaquePsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(shaders_["opaquePS"]->GetBufferPointer()),
			shaders_["opaquePS"]->GetBufferSize()
		};
		opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		opaquePsoDesc.SampleMask = UINT_MAX;
		opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		opaquePsoDesc.NumRenderTargets = 1;
		opaquePsoDesc.RTVFormats[0] = backBufferFormat_;
		opaquePsoDesc.SampleDesc.Count = _4xMsaaState ? 4 : 1;
		opaquePsoDesc.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
		opaquePsoDesc.DSVFormat = depthStencilFormat_;
		ThrowIfFailed(d3dDevice_->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&PSOs_["opaque"])));


		//
		// PSO for opaque wireframe objects.
		//

		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
		opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowIfFailed(d3dDevice_->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&PSOs_["opaque_wireframe"])));
	}

	void RenderD12::updateCamera(float radius, float phi, float theta)
	{
		eyePos_.x = radius * sinf(phi) * cosf(theta);
		eyePos_.z = radius * sinf(phi) * sinf(theta);
		eyePos_.y = radius * cosf(phi);

		// Build the view matrix.
		XMVECTOR pos = XMVectorSet(eyePos_.x, eyePos_.y, eyePos_.z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&mView, view);
	}

	void RenderD12::startDraw()
	{
		// Cycle through the circular frame resource array.
		currFrameResourceIndex_ = (currFrameResourceIndex_ + 1) % gNumFrameResources;
		currFrameResource_ = frameResources_[currFrameResourceIndex_].get();

		// Has the GPU finished processing the commands of the current frame resource?
		// If not, wait until the GPU has completed commands up to this fence point.
		if (currFrameResource_->Fence != 0 && fence_->GetCompletedValue() < currFrameResource_->Fence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(fence_->SetEventOnCompletion(currFrameResource_->Fence, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		auto cmdListAlloc = currFrameResource_->CmdListAlloc;

		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		ThrowIfFailed(cmdListAlloc->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.


		ThrowIfFailed(commandList_->Reset(cmdListAlloc.Get(), PSOs_["opaque"].Get()));		
	}

	void RenderD12::UpdateObjectCBs()
	{
		auto currObjectCB = currFrameResource_->ObjectCB.get();
		for (auto& e : opaqueRitems_)
		{
			// Only update the cbuffer data if the constants have changed.  
			// This needs to be tracked per frame resource.
			if (e->NumFramesDirty > 0)
			{
				XMMATRIX world = XMLoadFloat4x4(&e->World);

				ObjectConstants objConstants;
				XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

				currObjectCB->CopyData(e->ObjCBIndex, objConstants);

				// Next FrameResource need to be updated too.
				e->NumFramesDirty--;
			}
		}
	}

	void RenderD12::UpdateMainPassCB()
	{
		XMMATRIX view = XMLoadFloat4x4(&mView);
		XMMATRIX proj = XMLoadFloat4x4(&mProj);

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&mainPassCB_.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&mainPassCB_.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&mainPassCB_.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&mainPassCB_.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&mainPassCB_.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&mainPassCB_.InvViewProj, XMMatrixTranspose(invViewProj));
		mainPassCB_.EyePosW = eyePos_;
		mainPassCB_.RenderTargetSize = XMFLOAT2((float)clientWidth_, (float)clientHeight_);
		mainPassCB_.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth_, 1.0f / clientHeight_);
		mainPassCB_.NearZ = 1.0f;
		mainPassCB_.FarZ = 1000.0f;
		//mainPassCB_.TotalTime = gt.TotalTime();
		//mainPassCB_.DeltaTime = gt.DeltaTime();

		auto currPassCB = currFrameResource_->PassCB.get();
		currPassCB->CopyData(0, mainPassCB_);
	}

	void RenderD12::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
	{
		UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		auto objectCB = currFrameResource_->ObjectCB->Resource();

		// For each render item...
		for (size_t i = 0; i < ritems.size(); ++i)
		{
			auto ri = ritems[i];

			cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
			cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
			cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

			// Offset to the CBV in the descriptor heap for this object and for this frame resource.
			UINT cbvIndex = currFrameResourceIndex_ * (UINT)opaqueRitems_.size() + ri->ObjCBIndex;
			auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap_->GetGPUDescriptorHandleForHeapStart());
			cbvHandle.Offset(cbvIndex, cbvSrvUavDescriptorSize_);

			cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);

			cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		}
	} 

	std::unique_ptr<MeshGeometry>  RenderD12::createGeometry(const char* name,  const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices)
	{
		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = name;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(d3dDevice_.Get(),
			commandList_.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

		geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(d3dDevice_.Get(),
			commandList_.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(Vertex);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		return geo;
	}

	void RenderD12::setOpaque(const std::vector<RenderItem*>& opaqueRitems)
	{
		opaqueRitems_ = opaqueRitems;
	}

}

