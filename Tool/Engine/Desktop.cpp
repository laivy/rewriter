#include "Stdafx.h"
#include "Desktop.h"
#include "UIEditor.h"

void Desktop::Render()
{
	RenderMainMenuBar();
	RenderMainDockSpace();
}

void Desktop::RenderMainMenuBar()
{
	if (!ImGui::BeginMainMenuBar())
		return;

	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("UIEditor", nullptr, UIEditor::GetInstance()))
		{
			if (UIEditor::IsInstanced())
				UIEditor::Destroy();
			else
				UIEditor::Instantiate();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void Desktop::RenderMainDockSpace()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DOCKSPACE", NULL,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);
	ImGui::DockSpace(ImGui::GetID("DOCKSPACE"), {}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}
