#pragma once
#include "UI.h"

class Wnd : public UI
{
public:
	Wnd(FLOAT width, FLOAT height, FLOAT x = 0.0f, FLOAT y = 0.0f);
	virtual ~Wnd() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnButtonClicked(INT id);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget);

	template <typename T>
	void AddUI(std::unique_ptr<T>& ui)
	{
		std::unique_ptr<UI> _ui{ static_cast<UI*>(ui.release()) };
		_ui->SetParent(this);
		m_ui.push_back(std::move(_ui));
	}

	void SetFocus(BOOL isFocus);
	void SetPick(BOOL isPick);

	std::mutex& GetLock();
	BOOL IsValid() const;
	BOOL IsFocus() const;
	BOOL IsPick() const;
	FLOAT2 GetPickedDelta() const;

private:
	std::mutex m_mutex; // 락

	BOOL m_isFocus;	// 활성화 여부
	BOOL m_isPick; // 선택됨 여부(마우스로 창을 움직임)
	FLOAT2 m_pickDelta; // 창 좌표 - 선택됐을 때의 마우스 좌표

	std::vector<std::unique_ptr<UI>> m_ui; // 이 윈도우에 있는 UI 객체들
};