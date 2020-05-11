#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3d12Utils/d3dUtil.h"
#include "d3d12Utils/UploadBuffer.h"
#include "d3d12Utils/MathHelper.h"
#include "d3d12Utils/FrameResource.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")



namespace render
{
	struct RenderItem
	{
		RenderItem() = default;

		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify obect data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty = gNumFrameResources;

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex = -1;

		MeshGeometry* Geo = nullptr;

		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};



	class RenderD12
	{
	public:		
		RenderD12();

		bool initialize(HWND hMainWnd);

		
		void startDraw();
		void draw();

		void resize();
		void updateCamera(float radius, float phi, float theta);
		

		void setOpaque(const std::vector<RenderItem*>& opaqueRitems);
		std::unique_ptr<MeshGeometry> createGeometry(const char* name, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);

	private:
		void FlushCommandQueue();

		void CreateCommandObjects();
		void CreateSwapChain();
		void CreateRtvAndDsvDescriptorHeaps();


		void BuildRootSignature();
		void BuildShadersAndInputLayout();
		void BuildShapeGeometry();
		void BuildRenderItems();
		void BuildFrameResources();
		void BuildDescriptorHeaps();
		void BuildConstantBufferViews();
		void BuildPSOs();

		void UpdateObjectCBs();
		void UpdateMainPassCB();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap_;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DirectCmdListAlloc_;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory_;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice_;

		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

		D3D12_VIEWPORT screenViewport_;
		D3D12_RECT scissorRect_;

		UINT passCbvOffset_ = 0;
		UINT RtvDescriptorSize_ = 0;
		UINT DsvDescriptorSize_ = 0;
		UINT cbvSrvUavDescriptorSize_ = 0;

		DXGI_FORMAT depthStencilFormat_ = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT backBufferFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		

		// Set true to use 4X MSAA
		bool      _4xMsaaState = false;    // 4X MSAA enabled
		UINT      _4xMsaaQuality = 0;      // quality level of 4X MSAA

		static const int SwapChainBufferCount = 2;
		int currBackBuffer_ = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> SwapChainBuffer_[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> DepthStencilBuffer_;

		UINT64 currentFence_ = 0;		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap_{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_{ nullptr };

		std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB_{ nullptr };

		std::unique_ptr<MeshGeometry> BoxGeo_ = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout_;
		Microsoft::WRL::ComPtr<ID3DBlob> vsByteCode_ = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> psByteCode_ = nullptr;

				
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders_;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOs_;

		// from window 
		HWND      hMainWnd_ = nullptr; // main window handle
		int clientWidth_ = 800;
		int clientHeight_ = 600;

		// camera
		DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 eyePos_ = { 0.0f, 0.0f, 0.0f };
		

		DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();		
		std::vector<std::unique_ptr<FrameResource>> frameResources_;
		FrameResource* currFrameResource_ = nullptr;
		int currFrameResourceIndex_ = 0;
		
		// Render items divided by PSO.
		const int maxOpaqueItems_ = 22;
		std::vector<RenderItem*> opaqueRitems_;

		PassConstants mainPassCB_;
	};

}