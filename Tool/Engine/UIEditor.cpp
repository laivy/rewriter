#include "Stdafx.h"
#include "App.h"
#include "UIEditor.h"
#include "Common/Util.h"

UIEditor::UIEditor() :
	m_isFirstRender{ true },
	m_isVisible{},
	m_imguiWindowRect{},
	m_window{}
{
	App::OnPropertyModified.Register(this, std::bind_front(&UIEditor::OnPropertyModified, this));
}

void UIEditor::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
		m_isVisible = true;
		DragDrop();
		if (ImGui::BeginChild("VIEWER"))
			RenderViewer();
		ImGui::EndChild();
	}
	else
	{
		m_isVisible = false;
	}
	ImGui::End();
	ImGui::PopID();

	if (m_isFirstRender)
		m_isFirstRender = false;
}

void UIEditor::Render2D()
{
	if (!m_isVisible)
		return;

	auto& layer{ m_window.layer };
	if (!layer)
		return;

	layer->Begin();

	// 배경
	RectF rect{ 0.0f, 0.0f, static_cast<float>(m_window.size.x), static_cast<float>(m_window.size.y) };
	Float2 radius{ m_window.backgroundRectRadius };
	Graphics::D2D::DrawRoundRect(rect, radius, m_window.backgroundColor);

	layer->End();

	Graphics::D2D::PushClipRect(m_imguiWindowRect);
	layer->Draw(Float2{ m_imguiWindowRect.left, m_imguiWindowRect.top } - m_window.size / 2.0f + m_window.camera.position);
	Graphics::D2D::PopClipRect();
}

void UIEditor::OnPropertyModified(const std::shared_ptr<Resource::Property>& prop)
{
	if (m_window.prop == prop)
	{
		BuildWindow(m_window.prop);
		return;
	}

	auto parent{ prop.get() };
	while (parent = parent->GetParent())
	{
		if (m_window.prop.get() == parent)
		{
			BuildWindow(m_window.prop);
			return;
		}
	}
}

void UIEditor::RenderViewer()
{
	if (m_isFirstRender)
	{
		auto window{ ImGui::GetCurrentWindow() };
		m_window.camera.position = Float2{ window->SizeFull.x, window->SizeFull.y } / 2.0f;
	}

	UpdateImguiWindowRect();

	// 배경
	ImDrawList* drawList{ ImGui::GetWindowDrawList() };
	drawList->AddRectFilled(ImVec2{ m_imguiWindowRect.left, m_imguiWindowRect.top }, ImVec2{ m_imguiWindowRect.right, m_imguiWindowRect.bottom }, IM_COL32(50, 50, 50, 255));

	// 격자
	constexpr auto GRID_STEP{ 100.0f };
	float width{ m_imguiWindowRect.right - m_imguiWindowRect.left };
	float height{ m_imguiWindowRect.bottom - m_imguiWindowRect.top };
	for (float x{ std::fmodf(m_window.camera.position.x, GRID_STEP) }; x < width; x += GRID_STEP)
		drawList->AddLine(ImVec2(m_imguiWindowRect.left + x, m_imguiWindowRect.top), ImVec2(m_imguiWindowRect.left + x, m_imguiWindowRect.bottom), IM_COL32(200, 200, 200, 40));
	for (float y{ std::fmodf(m_window.camera.position.y, GRID_STEP) }; y < height; y += GRID_STEP)
		drawList->AddLine(ImVec2(m_imguiWindowRect.left, m_imguiWindowRect.top + y), ImVec2(m_imguiWindowRect.right, m_imguiWindowRect.top + y), IM_COL32(200, 200, 200, 40));

	// (0, 0) 격자
	drawList->AddLine(
		ImVec2{ m_imguiWindowRect.left + m_window.camera.position.x, m_imguiWindowRect.top },
		ImVec2{ m_imguiWindowRect.left + m_window.camera.position.x, m_imguiWindowRect.bottom },
		IM_COL32(200, 200, 200, 255)
	);
	drawList->AddLine(
		ImVec2{ m_imguiWindowRect.left, m_imguiWindowRect.top + m_window.camera.position.y },
		ImVec2{ m_imguiWindowRect.right, m_imguiWindowRect.top + m_window.camera.position.y },
		IM_COL32(200, 200, 200, 255)
	);
}

void UIEditor::DragDrop()
{
	auto window{ ImGui::GetCurrentWindow() };
	if (!ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		return;

	auto payload{ ImGui::AcceptDragDropPayload("HIERARCHY/PROPERTY") };
	if (!payload)
		return;

	auto prop{ *reinterpret_cast<std::shared_ptr<Resource::Property>*>(payload->Data) };
	if (!prop)
		return;

	// 전달받은 프로퍼티를 기반으로 윈도우 생성
	m_window = Window{};
	m_window.camera.position = Float2{ window->SizeFull.x, window->SizeFull.y } / 2.0f;
	BuildWindow(prop);

	App::OnPropertySelected.Notify(prop);
}

void UIEditor::BuildWindow(const std::shared_ptr<Resource::Property>& prop)
{
	m_window.prop = prop;

	std::wstring path{ prop->GetName() };
	auto parent{ prop.get() };
	while (parent = parent->GetParent())
		path = parent->GetName() + L"/" + path;
	m_window.path = path;

	if (auto info{ prop->Get(L"Info") })
	{
		m_window.size = info->GetInt2(L"Size");
		m_window.backgroundColor = Graphics::D2D::Color{ static_cast<uint32_t>(info->GetInt(L"BackgroundColor")) };
		m_window.backgroundRectRadius = info->GetInt2(L"BackgroundRadius");
		m_window.layer = Graphics::D2D::CreateLayer(m_window.size);
	}
}

void UIEditor::UpdateImguiWindowRect()
{
	auto window{ ImGui::GetCurrentWindow() };
	auto pos{ window->Pos };
	auto size{ window->SizeFull };
	m_imguiWindowRect = RectF{ 0.0f, 0.0f, size.x, size.y }.Offset(Float2{ pos.x, pos.y });

	ImGui::PushID("hello");
	ImGui::InvisibleButton("", size, ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
	auto& io{ ImGui::GetIO() };
	if (ImGui::IsItemActive())
	{
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
		{
			m_window.camera.position += Float2{ io.MouseDelta.x, io.MouseDelta.y };
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		{
			// (0, 0)이 윈도우 가운데에 오도록 이동
			m_window.camera.position = Float2{ size.x, size.y } / 2.0f;
		}
	}
	ImGui::PopID();
}
