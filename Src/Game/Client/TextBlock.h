#pragma once
#include "Control.h"

class TextBlock final : public IControl
{
public:
	TextBlock(IWindow* owner, const Resource::Property::ID id = nullptr);
	~TextBlock() = default;

	void Render() const override;

	void SetText(std::wstring_view text);

private:
	void Build(const Resource::Property::ID id);

private:
	std::wstring m_text;
	Graphics::D2D::Font m_font;
};
