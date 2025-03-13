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

		std::wstring path;
		std::weak_ptr<Resource::Property> prop;
		std::shared_ptr<Graphics::D2D::Layer> layer;
	};

	struct Camera
	{
		Float2 position;
		float scale{ 1.0f };
	};

public:
	UIEditor();
	~UIEditor() = default;

	void Render();
	void Render2D();

private:
	void OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertyModified(const std::shared_ptr<Resource::Property>& prop);

	void UpdateImguiWindowRect();
	void RenderTopBar();
	void RenderViewer();

	void DragDrop();
	void BuildWindow(const std::shared_ptr<Resource::Property>& prop);

private:
	static constexpr auto WINDOW_NAME{ "UI Editor" };

	bool m_isVisible;
	RectF m_viewerRect;

	Camera m_camera;
	bool m_moveCameraToCenter;

	std::unique_ptr<Window> m_window;
};
