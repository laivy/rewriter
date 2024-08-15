#pragma once

// C/C++
#include <string>

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

// ImGui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Project
#include "Util.h"

namespace ImGui
{
	void OnResize(int width, int height);

	// Standalone 초기화
	void Init(HWND _hWnd, ImGuiConfigFlags configFlags);

	// 이미 생성된 DX12 디바이스로 초기화
	void Init(HWND _hWnd, ID3D12Device* device, ID3D12GraphicsCommandList* _commandList, int num_frames_in_flight, DXGI_FORMAT rtv_format, ImGuiConfigFlags configFlags);

	void BeginRender();
	void EndRender();
	void CleanUp();

	// ImGui 유틸 함수
	template<class... Args>
	void Text(std::wstring_view fmt, Args... args)
	{
		auto utf8Fmt{ Util::wstou8s(fmt) };
		Text(utf8Fmt.c_str(), args...);
	}

	bool Button(std::wstring_view label, const ImVec2& size = ImVec2(0, 0));
}
