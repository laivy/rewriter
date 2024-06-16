#pragma once
#include <d3d12.h>

namespace ImGui
{
#ifdef _IMGUI_STANDALONE
	void Init(HWND hWnd);
	void OnResize(int width, int height);
#else
	void Init(HWND hWnd = nullptr, ID3D12Device* device = nullptr, int num_frames_in_flight = 0, DXGI_FORMAT rtv_format = DXGI_FORMAT_UNKNOWN,
		ID3D12DescriptorHeap* cbv_srv_heap = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle = {}, D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle = {});
#endif
	void RenderBegin();
	void RenderEnd();
	void CleanUp();

}