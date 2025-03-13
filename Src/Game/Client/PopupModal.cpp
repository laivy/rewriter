#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "PopupModal.h"
#include "TextBlock.h"

PopupModal::PopupModal(std::wstring_view text) :
	IModal{ L"UI.dat/Popup" }
{
	constexpr auto WINDOW_WIDTH_MIN{ 200 };
	constexpr auto WINDOW_HEIGHT_MIN{ 80 };
	constexpr auto PADDING{ 20 };

	Int2 windowSize{ WINDOW_WIDTH_MIN, WINDOW_HEIGHT_MIN };

	if (auto textBlock{ GetControl<TextBlock>(L"Text") })
	{
		textBlock->SetText(text);
		auto textSize{ textBlock->GetSize() };
		windowSize.x = std::max(WINDOW_WIDTH_MIN, textSize.x + PADDING);
		windowSize.y = std::max(WINDOW_HEIGHT_MIN, textSize.y + PADDING);
	}

	if (auto button{ GetControl<Button>(L"OK") })
	{
		button->OnButtonClick.Register([this]() { Return(Result::Ok); });
		button->SetPosition(Int2{ windowSize.x / 2, windowSize.y + PADDING / 2 }, Pivot::CenterBot);
		windowSize.y = std::max(WINDOW_HEIGHT_MIN, windowSize.y + 20);
	}

	SetSize(windowSize);
	SetPosition(App::size / 2, Pivot::Center);
}
