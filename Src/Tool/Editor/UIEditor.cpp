#include "Pch.h"
#include "App.h"
#include "Delegates.h"
#include "UIEditor.h"
#include "Common/Util.h"

#if 0
UIEditor::UIEditor() :
	m_isVisible{ false },
	m_viewerRect{},
	m_moveCameraToCenter{ true }
{
	Delegates::OnPropertyDeleted.Register(this, std::bind_front(&UIEditor::OnPropertyDeleted, this));
	Delegates::OnPropertyModified.Register(this, std::bind_front(&UIEditor::OnPropertyModified, this));
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

	Graphics::D2D::PushLayer(m_window->layer);
	m_window->Render();
	Graphics::D2D::PopLayer();

	Graphics::D2D::Scale scale{};
	scale.scale = Float2{ m_camera.scale, m_camera.scale };

	Float2 translation{ m_viewerRect.left, m_viewerRect.top };
	translation -= Float2{ m_window->GetSize() } * m_camera.scale / 2.0f;
	translation += m_camera.position;

	Graphics::D2D::PushClipRect(m_viewerRect);
	Graphics::D2D::SetTransform(Graphics::D2D::Transform{ .scale = scale, .translation = translation });
	Graphics::D2D::DrawLayer(m_window->layer);
	Graphics::D2D::SetTransform(Graphics::D2D::Transform{});
	Graphics::D2D::PopClipRect();
}

void UIEditor::OnPropertyDeleted(const std::shared_ptr<Resource::Property>& prop)
{
	/* TODO
	if (!m_window)
		return;

	if (m_window->prop.lock() == prop)
	{
		m_window.reset();
		m_moveCameraToCenter = true;
		return;
	}

	auto parent{ prop };
	while (parent = parent->GetParent())
	{
		if (m_window->prop.lock() == parent)
		{
			BuildWindow(parent);
			return;
		}
	}
	*/
}

void UIEditor::OnPropertyModified(const std::shared_ptr<Resource::Property>& prop)
{
	/* TODO
	if (!m_window)
		return;

	if (m_window->prop.lock() == prop)
	{
		BuildWindow(prop);
		return;
	}

	auto parent{ prop };
	while (parent = parent->GetParent())
	{
		if (m_window->prop.lock() == parent)
		{
			BuildWindow(parent);
			return;
		}
	}
	*/
}

void UIEditor::UpdateImguiWindowRect()
{
	auto window{ ImGui::GetCurrentWindow() };
	auto pos{ window->Pos };
	auto size{ window->SizeFull };
	m_viewerRect = RectF{ 0.0f, 0.0f, size.x, size.y }.Offset(Float2{ pos.x, pos.y });

	ImGui::InvisibleButton("CANVAS", size, ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
	auto& io{ ImGui::GetIO() };
	if (ImGui::IsItemActive())
	{
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
		{
			m_camera.position += Float2{ io.MouseDelta.x, io.MouseDelta.y };
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		{
			m_camera.position = Float2{ size.x, size.y } / 2.0f;
			m_camera.scale = 1.0f;
		}
	}

	if (m_viewerRect.Contains(Float2{ io.MousePos.x, io.MousePos.y }) && ImGui::IsKeyDown(ImGuiKey_MouseWheelY))
	{
		constexpr auto SCALE_MIN{ 0.1f };
		constexpr auto SCALE_MAX{ 10.0f };
		if (io.MouseWheel > 0)
			m_camera.scale = std::min(SCALE_MAX, m_camera.scale * 1.1f);
		else
			m_camera.scale = std::max(SCALE_MIN, m_camera.scale * 0.9f);
	}
}

void UIEditor::RenderTopBar()
{
	if (!m_window)
		return;

	ImGui::Text(Util::wstou8s(m_window->path).c_str());

	Int2 mouse{ App::GetInstance()->GetCursorPosition() };
	if (m_viewerRect.Contains(Float2{ mouse.x, mouse.y }))
	{
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::TextUnformatted(std::format("{:.2f}%", m_camera.scale * 100.0f).c_str());

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

		ImGui::SameLine();
		ImGui::TextUnformatted(
			std::format("x: {}, y: {}",
				static_cast<int>((mouse.x - m_viewerRect.left - m_camera.position.x) / m_camera.scale),
				static_cast<int>((mouse.y - m_viewerRect.top - m_camera.position.y) / m_camera.scale)
			).c_str()
		);
	}
}

void UIEditor::RenderViewer()
{
	UpdateImguiWindowRect();

	if (m_moveCameraToCenter)
	{
		auto imguiWindow{ ImGui::GetCurrentWindow() };
		m_camera.position = Float2{ imguiWindow->SizeFull.x, imguiWindow->SizeFull.y } / 2.0f;
		m_moveCameraToCenter = false;
	}

	// 배경
	auto drawList{ ImGui::GetWindowDrawList() };
	drawList->AddRectFilled(ImVec2{ m_viewerRect.left, m_viewerRect.top }, ImVec2{ m_viewerRect.right, m_viewerRect.bottom }, IM_COL32(50, 50, 50, 255));

	// 격자
	constexpr auto DEFAULT_GRID_STEP{ 100.0f };
	float gridStep{ DEFAULT_GRID_STEP * m_camera.scale };
	float width{ m_viewerRect.right - m_viewerRect.left };
	float height{ m_viewerRect.bottom - m_viewerRect.top };
	for (float x{ std::fmodf(m_camera.position.x, gridStep) }; x < width; x += gridStep)
	{
		drawList->AddLine(
			ImVec2{ m_viewerRect.left + x, m_viewerRect.top },
			ImVec2{ m_viewerRect.left + x, m_viewerRect.bottom },
			IM_COL32(200, 200, 200, 40)
		);
	}
	for (float y{ std::fmodf(m_camera.position.y, gridStep) }; y < height; y += gridStep)
	{
		drawList->AddLine(
			ImVec2{ m_viewerRect.left, m_viewerRect.top + y },
			ImVec2{ m_viewerRect.right, m_viewerRect.top + y },
			IM_COL32(200, 200, 200, 40)
		);
	}

	// (0, 0) 격자
	drawList->AddLine(
		ImVec2{ m_viewerRect.left + m_camera.position.x, m_viewerRect.top },
		ImVec2{ m_viewerRect.left + m_camera.position.x, m_viewerRect.bottom },
		IM_COL32(200, 200, 200, 255)
	);
	drawList->AddLine(
		ImVec2{ m_viewerRect.left, m_viewerRect.top + m_camera.position.y },
		ImVec2{ m_viewerRect.right, m_viewerRect.top + m_camera.position.y },
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
	m_moveCameraToCenter = true;

	Delegates::OnPropertySelected.Notify(prop);
}

void UIEditor::BuildWindow(const std::shared_ptr<Resource::Property>& prop)
{
	/* TODO
	m_window = std::make_unique<Window>(prop);
	m_window->prop = prop;
	m_window->layer = Graphics::D2D::CreateLayer(m_window->GetSize());

	std::wstring path{ prop->GetName() };
	auto parent{ prop };
	while (parent = parent->GetParent())
		path = std::format(L"{}/{}", parent->GetName(), path);
	m_window->path = path;
	*/
}

UIEditor::Window::Window(const std::shared_ptr<Resource::Property>& prop) :
	IWindow{ prop }
{
}
#endif
