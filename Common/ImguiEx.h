#pragma once
#include <Windows.h>
#include "External/Imgui/imgui.h"
#ifndef _IMGUI_STANDALONE
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

namespace ImGui
{
#ifdef _IMGUI_STANDALONE
	void Init(HWND _hWnd, ImGuiConfigFlags configFlags);
	void OnResize(int width, int height);
#else
	void Init(HWND _hWnd, ID3D12Device* device, ID3D12GraphicsCommandList* _commandList, int num_frames_in_flight, DXGI_FORMAT rtv_format, ImGuiConfigFlags configFlags);
#endif
	void BeginRender();
	void EndRender();
	void CleanUp();
}