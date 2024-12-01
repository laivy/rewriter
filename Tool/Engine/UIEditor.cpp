#include "Stdafx.h"
#include "App.h"
#include "UIEditor.h"
#include "Common/Util.h"

UIEditor::UIEditor() :
	m_isVisible{},
	m_moveCameraToCenter{ true },
	m_imguiWindowRect{}
{
	App::OnPropertyDelete.Register(this, std::bind_front(&UIEditor::OnPropertyDelete, this));
	App::OnPropertyModified.Register(this, std::bind_front(&UIEditor::OnPropertyModified, this));
}

void UIEditor::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
		m_isVisible = true;
		DragDrop();
		if (ImGui::BeginChild("TOPBAR", ImVec2{ 0.0f, 20.0f }))
			RenderTopBar();
		ImGui::EndChild();
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
}

void UIEditor::Render2D()
{
	if (!m_window || !m_window->layer || !m_isVisible)
		return;

	m_window->layer->Begin();
	m_window->Render();
	m_window->layer->End();

	Graphics::D2D::Scale scale{};
	scale.scale = Float2{ m_window->camera.scale, m_window->camera.scale };

	Float2 translation{ m_imguiWindowRect.left, m_imguiWindowRect.top };
	translation -= Float2{ m_window->GetSize() } * m_window->camera.scale / 2.0f;
	translation += m_window->camera.position;

	Graphics::D2D::PushClipRect(m_imguiWindowRect);
	Graphics::D2D::SetTransform(Graphics::D2D::Transform{ .scale = scale, .translation = translation });
	m_window->layer->Draw();
	Graphics::D2D::SetTransform(Graphics::D2D::Transform{});
	Graphics::D2D::PopClipRect();
}

void UIEditor::OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop)
{
	if (!m_window)
		return;

	if (m_window->prop == prop)
	{
		m_window.reset();
		m_moveCameraToCenter = true;
		return;
	}

	auto parent{ prop.get() };
	while (parent = parent->GetParent())
	{
		if (m_window->prop.get() == parent)
		{
			auto temp{ m_window->prop };
			BuildWindow(temp);
			return;
		}
	}
}

void UIEditor::OnPropertyModified(const std::shared_ptr<Resource::Property>& prop)
{
	if (!m_window)
		return;

	if (m_window->prop == prop)
	{
		BuildWindow(m_window->prop);
		return;
	}

	auto parent{ prop.get() };
	while (parent = parent->GetParent())
	{
		if (m_window->prop.get() == parent)
		{
			auto temp{ m_window->prop };
			BuildWindow(temp);
			return;
		}
	}
}

void UIEditor::RenderTopBar()
{
	if (!m_window)
		return;

	if (m_window->prop)
	{
		ImGui::Text(Util::wstou8s(m_window->path).c_str());
	}

	POINT mouse{};
	::GetCursorPos(&mouse);
	::ScreenToClient(App::hWnd, &mouse);
	if (m_imguiWindowRect.Contains(Float2{ mouse.x, mouse.y }))
	{
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::TextUnformatted(std::format("{:.2f}%", m_window->camera.scale * 100.0f).c_str());

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::TextUnformatted(
			std::format("x: {}, y: {}",
				static_cast<int>((mouse.x - m_imguiWindowRect.left - m_window->camera.position.x) / m_window->camera.scale),
				static_cast<int>((mouse.y - m_imguiWindowRect.top - m_window->camera.position.y) / m_window->camera.scale)
			).c_str()
		);
	}
}

void UIEditor::RenderViewer()
{
	UpdateImguiWindowRect();

	auto imguiWindow{ ImGui::GetCurrentWindow() };
	if (m_window && m_moveCameraToCenter)
	{
		m_window->camera.position = Float2{ imguiWindow->SizeFull.x, imguiWindow->SizeFull.y } / 2.0f;
		m_moveCameraToCenter = false;
	}

	Float2 cameraPosition{};
	if (m_window)
	{
		cameraPosition = m_window->camera.position;
	}
	else
	{
		cameraPosition = Float2{ imguiWindow->SizeFull.x, imguiWindow->SizeFull.y } / 2.0f;
	}

	// 배경
	ImDrawList* drawList{ ImGui::GetWindowDrawList() };
	drawList->AddRectFilled(ImVec2{ m_imguiWindowRect.left, m_imguiWindowRect.top }, ImVec2{ m_imguiWindowRect.right, m_imguiWindowRect.bottom }, IM_COL32(50, 50, 50, 255));

	// 격자
	constexpr auto DEFAULT_GRID_STEP{ 100.0f };
	float gridStep{ DEFAULT_GRID_STEP * (m_window ? m_window->camera.scale : 1.0f) };
	float width{ m_imguiWindowRect.right - m_imguiWindowRect.left };
	float height{ m_imguiWindowRect.bottom - m_imguiWindowRect.top };
	for (float x{ std::fmodf(cameraPosition.x, gridStep) }; x < width; x += gridStep)
		drawList->AddLine(ImVec2(m_imguiWindowRect.left + x, m_imguiWindowRect.top), ImVec2(m_imguiWindowRect.left + x, m_imguiWindowRect.bottom), IM_COL32(200, 200, 200, 40));
	for (float y{ std::fmodf(cameraPosition.y, gridStep) }; y < height; y += gridStep)
		drawList->AddLine(ImVec2(m_imguiWindowRect.left, m_imguiWindowRect.top + y), ImVec2(m_imguiWindowRect.right, m_imguiWindowRect.top + y), IM_COL32(200, 200, 200, 40));

	// (0, 0) 격자
	drawList->AddLine(
		ImVec2{ m_imguiWindowRect.left + cameraPosition.x, m_imguiWindowRect.top },
		ImVec2{ m_imguiWindowRect.left + cameraPosition.x, m_imguiWindowRect.bottom },
		IM_COL32(200, 200, 200, 255)
	);
	drawList->AddLine(
		ImVec2{ m_imguiWindowRect.left, m_imguiWindowRect.top + cameraPosition.y },
		ImVec2{ m_imguiWindowRect.right, m_imguiWindowRect.top + cameraPosition.y },
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

	BuildWindow(prop);
	App::OnPropertySelected.Notify(prop);
}

void UIEditor::BuildWindow(const std::shared_ptr<Resource::Property>& prop)
{
	m_moveCameraToCenter = true;

	m_window = std::make_unique<Window>(prop);
	m_window->prop = prop;

	std::wstring path{ prop->GetName() };
	auto parent{ prop.get() };
	while (parent = parent->GetParent())
		path = parent->GetName() + L"/" + path;
	m_window->path = path;

	m_window->layer = Graphics::D2D::CreateLayer(m_window->GetSize());
}

void UIEditor::UpdateImguiWindowRect()
{
	auto window{ ImGui::GetCurrentWindow() };
	auto pos{ window->Pos };
	auto size{ window->SizeFull };
	m_imguiWindowRect = RectF{ 0.0f, 0.0f, size.x, size.y }.Offset(Float2{ pos.x, pos.y });

	if (m_window)
	{
		ImGui::InvisibleButton("CANVAS", size, ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
		auto& io{ ImGui::GetIO() };
		if (ImGui::IsItemActive())
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
			{
				m_window->camera.position += Float2{ io.MouseDelta.x, io.MouseDelta.y };
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
			{
				m_window->camera.position = Float2{ size.x, size.y } / 2.0f;
				m_window->camera.scale = 1.0f;
			}
		}
		if (ImGui::IsKeyDown(ImGuiKey_MouseWheelY))
		{
			constexpr auto SCALE_MIN{ 0.1f };
			constexpr auto SCALE_MAX{ 10.0f };
			if (io.MouseWheel > 0)
				m_window->camera.scale = std::min(SCALE_MAX, m_window->camera.scale * 1.1f);
			else
				m_window->camera.scale = std::max(SCALE_MIN, m_window->camera.scale * 0.9f);
		}
	}
}

UIEditor::Window::Window(const std::shared_ptr<Resource::Property>& prop) :
	IWindow{ prop }
{
}
