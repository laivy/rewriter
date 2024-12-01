#pragma once
#include "Game/Client/Window.h"

class UIEditor :
	public IObserver,
	public TSingleton<UIEditor>
{
private:
	class Window : public IWindow
	{
	public:
		Window(const std::shared_ptr<Resource::Property>& prop);
		~Window() = default;

	public:
		struct Camera
		{
			Float2 position;
			float scale{ 1.0f };
		};

		std::shared_ptr<Resource::Property> prop;
		std::wstring path;
		std::shared_ptr<Graphics::D2D::Layer> layer;
		Camera camera;
	};

public:
	UIEditor();
	~UIEditor() = default;

	void Render();
	void Render2D();

private:
	void OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertyModified(const std::shared_ptr<Resource::Property>& prop);

	void RenderTopBar();
	void RenderViewer();
	void UpdateImguiWindowRect();

	void DragDrop();
	void BuildWindow(const std::shared_ptr<Resource::Property>& prop);

private:
	static constexpr auto WINDOW_NAME{ "UI Editor" };
	bool m_isVisible;
	bool m_moveCameraToCenter;
	RectF m_imguiWindowRect;

	std::unique_ptr<Window> m_window;
};
