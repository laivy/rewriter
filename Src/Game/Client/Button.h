#pragma once
#include "Control.h"

class Button : public IControl
{
private:
	using Visual = std::variant<std::shared_ptr<Resource::Sprite>, std::tuple<Int2, Float2, Graphics::D2D::Color>>;

	enum class State
	{
		Normal,
		Hover,
		Active,
		Disable
	};

public:
	Button(IWindow* owner, const Resource::Property::ID id = nullptr);
	~Button() = default;

	virtual void OnMouseEnter(int x, int y) override final;
	virtual void OnMouseLeave(int x, int y) override final;
	virtual void OnMouseEvent(UINT message, int x, int y) override final;

	virtual void Render() const override final;

private:
	void Build(const Resource::Property::ID id);

public:
	Delegate<> OnButtonClick;

private:
	State m_state;
	std::array<Visual, 4> m_visuals;
};
