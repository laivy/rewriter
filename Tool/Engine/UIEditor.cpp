#include "Stdafx.h"
#include "App.h"
#include "UIEditor.h"
#include "Common/Util.h"

UIEditor::UIEditor() :
	m_isVisible{false},
	m_clipRect{}
{
}

void UIEditor::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
		m_isVisible = true;
		DragDrop();
		if (ImGui::BeginChild("VIEWER"))
			CalcClipRect();
		ImGui::EndChild();
	}
	else
	{
		m_isVisible = false;
	}
	ImGui::End();
	ImGui::PopID();
}

void UIEditor::Render2D()
{
	if (!m_isVisible)
		return;

	Graphics::D2D::PushClipRect(m_clipRect);

	RectF rect{ 0.0f, 0.0f, static_cast<float>(m_window.size.x), static_cast<float>(m_window.size.y) };
	Float2 radius{ m_window.backgroundRectRadius };
	Graphics::D2D::DrawRoundRect(rect.Offset({ m_clipRect.left, m_clipRect.top }), radius, m_window.backgroundColor);

	//Graphics::D2D::DrawRect(RectF{ 0.0f, 0.0f, 9999.0f, 9999.0f }, Graphics::D2D::Color::Blue);
	//Graphics::D2D::DrawRect(contentRegion, Graphics::D2D::Color::Red);
	Graphics::D2D::PopClipRect();
}

void UIEditor::DragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (!ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		return;

	auto payload{ ImGui::AcceptDragDropPayload("PROPERTY") };
	if (!payload)
		return;

	auto prop{ reinterpret_cast<std::shared_ptr<Resource::Property>*>(payload->Data) };
	if (!prop)
		return;

	// 전달받은 프로퍼티를 기반으로 윈도우 생성
	m_prop = *prop;
	BuildWindow();

	App::OnPropertySelect.Notify(m_prop);
}

void UIEditor::BuildWindow()
{
	m_window = {};
	if (auto info{ m_prop->Get(L"Info") })
	{
		m_window.size = info->GetInt2(L"Size");
		m_window.backgroundColor = info->GetInt(L"BackgroundColor");
		m_window.backgroundRectRadius = info->GetInt2(L"BackgroundRadius");
	}
}

void UIEditor::CalcClipRect()
{
	auto window{ ImGui::GetCurrentWindow() };
	auto pos{ window->Pos };
	auto size{ window->SizeFull };
	POINT lt{ static_cast<long>(pos.x), static_cast<long>(pos.y) };
	POINT rb{ static_cast<long>(pos.x + size.x), static_cast<long>(pos.y + size.y) };
	::ScreenToClient(App::hWnd, &lt);
	::ScreenToClient(App::hWnd, &rb);
	m_clipRect = RectF{ static_cast<float>(lt.x), static_cast<float>(lt.y), static_cast<float>(rb.x), static_cast<float>(rb.y) };
}
