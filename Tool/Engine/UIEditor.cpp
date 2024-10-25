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
		ImGui::TextUnformatted(m_fullPath.c_str());
		ImGui::Separator();
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
	Graphics::D2D::DrawRect(RECTF{ 0.0f, 0.0f, 9999.0f, 9999.0f }, Graphics::D2D::Color::Blue);
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

	m_prop = *prop;
	auto fullPath{ m_prop->GetName() };
	for (auto parent{ m_prop->GetParent() }; parent != nullptr; parent = parent->GetParent())
		fullPath = std::format(L"{}{}{}", parent->GetName(), Stringtable::DATA_PATH_SEPERATOR, fullPath);
	m_fullPath = Util::wstou8s(fullPath);
	App::OnPropertySelect.Notify(m_prop);
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
	m_clipRect = RECTF{ static_cast<float>(lt.x), static_cast<float>(lt.y), static_cast<float>(rb.x), static_cast<float>(rb.y) };
}
