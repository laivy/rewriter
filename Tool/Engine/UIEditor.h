#pragma once

class UIEditor :
	public IObserver,
	public TSingleton<UIEditor>
{
private:
	struct Window
	{
		struct Camera
		{
			Float2 position;
			Float2 scale{ 1.0f, 1.0f };
		};

		std::shared_ptr<Resource::Property> prop;
		std::wstring path;

		std::shared_ptr<Graphics::D2D::Layer> layer;
		Camera camera;

		Int2 size;
		Graphics::D2D::Color backgroundColor;
		Int2 backgroundRectRadius;
	};

public:
	UIEditor();
	~UIEditor() = default;

	void Render();
	void Render2D();

private:
	void OnPropertyModified(const std::shared_ptr<Resource::Property>& prop);

	void RenderViewer();
	void UpdateImguiWindowRect();

	void DragDrop();
	void BuildWindow(const std::shared_ptr<Resource::Property>& prop);

private:
	static constexpr auto WINDOW_NAME{ "UI Editor" };
	bool m_isFirstRender;
	bool m_isVisible;
	RectF m_imguiWindowRect;

	Window m_window;
};
