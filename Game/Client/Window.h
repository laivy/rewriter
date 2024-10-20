#pragma once
#include "UI.h"

class IControl;

class IWindow abstract : public IUserInterface
{
	template<class T>
	requires std::is_base_of_v<IWindow, T>
	friend class WindowBuilder;

public:
	IWindow();
	virtual ~IWindow() = default;

	virtual void OnMouseLeave(int x, int y) override;
	virtual void OnMouseEvent(UINT message, int x, int y) override;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void Update(float deltaTime);
	virtual void Render() const;

	void Register(const std::shared_ptr<IControl>& control);

protected:
	std::shared_ptr<Graphics::D2D::Layer> GetLayer(int z);
	std::shared_ptr<Graphics::D2D::Layer> GetLayer(int z) const;

	template<class T = IControl>
	requires std::is_base_of_v<IControl, T>
	std::shared_ptr<T> GetControl(std::wstring_view	name) const
	{
		auto it{ std::ranges::find_if(m_controls, [name](const auto& control) { return control->GetName() == name; }) };
		if (it != m_controls.end())
			return std::static_pointer_cast<T>(*it);
		return nullptr;
	}

private:
	void UpdateMouseOverControl(int x, int y);
	void UpdateFocusControl(int x, int y);

	// WindowBuilder에서 사용하는 함수
	void SetNinePatch(const std::shared_ptr<Resource::Property>& prop);

private:
	RECTI m_titleBarRect;
	INT2 m_pickPos;
	bool m_isPicked;

	std::map<int, std::shared_ptr<Graphics::D2D::Layer>> m_layers;
	std::vector<std::shared_ptr<IControl>> m_controls;
	std::weak_ptr<IControl> m_mouseOverControl;
	std::weak_ptr<IControl> m_focusControl;

	std::array<std::shared_ptr<Resource::Sprite>, 9> m_ninePatch;
};
