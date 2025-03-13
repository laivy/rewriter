#pragma once
#include "Modal.h"

class PopupModal final : public IModal
{
public:
	PopupModal(std::wstring_view text);
	~PopupModal() = default;
};
