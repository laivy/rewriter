#pragma once
#include "UI.h"

class EditCtrl : public UI
{
public:
	EditCtrl(FLOAT width, FLOAT height, FLOAT x = 0.0f, FLOAT y = 0.0f);
	~EditCtrl() = default;

	virtual void Update(FLOAT deltaTime) { }
	virtual void Render(const ComPtr<ID2D1RenderTarget>& renderTarget);
	virtual RECTF GetRect() const;

private:

};