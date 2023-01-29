#pragma once

class Timer;

class NytApp : public TSingleton<NytApp>
{
public:
	NytApp();
	~NytApp() = default;

	void OnCreate();
	void OnDestroy();

	void Run();

	HWND GetHwnd() const;
	ComPtr<IDWriteFactory5> GetDwriteFactory() const;
	ComPtr<ID2D1HwndRenderTarget> GetRenderTarget() const;

private:
	HRESULT InitD2D();
	HRESULT InitWnd();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update();
	void Render();

	HRESULT CreateDeviceResources();

private:
	HWND								m_hwnd;
	ComPtr<ID2D1Factory>				m_d2dFactory;
	ComPtr<IDWriteFactory5>				m_dwriteFactory;
	ComPtr<ID2D1HwndRenderTarget>		m_renderTarget;

	std::unique_ptr<Timer>				m_timer;
};