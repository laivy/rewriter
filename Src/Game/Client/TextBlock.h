#pragma once
#include "Control.h"

class TextBlock final : public IControl
{
public:
	TextBlock(IWindow* owner, const std::shared_ptr<Resource::Property>& prop = nullptr);
	~TextBlock() = default;

	void Render() const override;

	void SetText(std::wstring_view text);

private:
	void Build(const std::shared_ptr<Resource::Property>& prop);

private:
	std::wstring m_text;
	Graphics::D2D::Font m_font;
};
