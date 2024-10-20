#pragma once
#include "Control.h"

class Button : public IControl
{
public:
	using Visual = std::variant<std::shared_ptr<Resource::Sprite>, std::tuple<FLOAT2, FLOAT2, int32_t>>;

private:
	enum class State
	{
		Normal,
		Hover,
		Active,
		Disable
	};

public:
	Button(IWindow* owner);
	~Button() = default;

	virtual void OnMouseEnter(int x, int y) override final;
	virtual void OnMouseLeave(int x, int y) override final;
	virtual void OnMouseEvent(UINT message, int x, int y) override final;

	virtual void Render() const override final;

	void SetVisuals(const Visual& normal, const Visual& hover, const Visual& active, const Visual& disable);

public:
	Delegate<> OnButtonClick;

private:
	State m_state;
	Visual m_normal;
	Visual m_hover;
	Visual m_active;
	Visual m_disable;
};
