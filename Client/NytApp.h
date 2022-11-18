#pragma once

template<class T>
class TSingleton;

class Timer;
class Wnd;
class WndManager;

class NytApp : public TSingleton<NytApp>
{
public:
	NytApp();
	~NytApp();

	void Run();
	HWND GetHwnd() const { return m_hwnd; }

private:
	HRESULT InitD2D();
	HRESULT InitWnd();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update();
	void Render();

	HRESULT CreateDeviceResources();

private:
	HWND								m_hwnd;
	ComPtr<ID2D1Factory>				m_D2DFactory;
	ComPtr<IWICImagingFactory>			m_WICFactory;
	ComPtr<IDWriteFactory>				m_DWriteFactory;
	ComPtr<ID2D1HwndRenderTarget>		m_renderTarget;
	ComPtr<IDWriteTextFormat>			m_textFormat;
	ComPtr<ID2D1PathGeometry>			m_pathGeometry;
	ComPtr<ID2D1LinearGradientBrush>	m_linearGradientBrush;
	ComPtr<ID2D1SolidColorBrush>		m_blackBrush;
	ComPtr<ID2D1BitmapBrush>			m_gridPatternBitmapBrush;
	ComPtr<ID2D1Bitmap>					m_bitmap;
	ComPtr<ID2D1Bitmap>					m_anotherBitmap;

	std::unique_ptr<Timer>				m_timer;
};