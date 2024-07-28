#pragma once
#include <Windows.h>
#include <string>
#include "Util.h"
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

	// ImGui 유틸 함수
	template<class... Args>
	void Text(std::wstring_view fmt, Args... args)
	{
		auto utf8Fmt{ Util::wstou8s(fmt) };
		Text(utf8Fmt.c_str(), args...);
	}

	bool Button(std::wstring_view label, const ImVec2& size = ImVec2(0, 0));
}