#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

//#include <WindowsX.h>

#include "d3d12Utils/d3dUtil.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace render
{
	class RenderD12
	{
	public:		
		RenderD12();

		bool initialize(HWND hMainWnd);
		void draw();

		void resize();


	private:
		void FlushCommandQueue();

		void CreateCommandObjects();
		void CreateSwapChain();
		void CreateRtvAndDsvDescriptorHeaps();


		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap_;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue_;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DirectCmdListAlloc_;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList_;

		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory_;
		Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain_;
		Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice_;

		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;

		D3D12_VIEWPORT ScreenViewport_;
		D3D12_RECT ScissorRect_;

		UINT RtvDescriptorSize_ = 0;
		UINT DsvDescriptorSize_ = 0;
		UINT CbvSrvUavDescriptorSize_ = 0;
		DXGI_FORMAT DepthStencilFormat_ = DXGI_FORMAT_D24_UNORM_S8_UINT;

		DXGI_FORMAT BackBufferFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Set true to use 4X MSAA (�4.1.8).  The default is false.
		bool      _4xMsaaState = false;    // 4X MSAA enabled
		UINT      _4xMsaaQuality = 0;      // quality level of 4X MSAA

		static const int SwapChainBufferCount = 2;
		int CurrBackBuffer_ = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> SwapChainBuffer_[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> DepthStencilBuffer_;

		UINT64 CurrentFence_ = 0;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap_{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature_{ nullptr };


		// from window 
		HWND      hMainWnd_ = nullptr; // main window handle
		int ClientWidth_ = 800;
		int ClientHeight_ = 600;
	};

}