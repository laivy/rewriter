#pragma once

class UIEditor : public TSingleton<UIEditor>
{
public:
	UIEditor();
	~UIEditor() = default;

	void Render();
	void Render2D();

private:
	void DragDrop();
	void CalcClipRect();

private:
	static constexpr auto WINDOW_NAME{ "UI Editor" };
	bool m_isVisible;
	RECTF m_clipRect;

	std::shared_ptr<Resource::Property> m_prop;
	std::string m_fullPath;
};
