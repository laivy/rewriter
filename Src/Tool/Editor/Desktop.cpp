#include "Stdafx.h"
#include "Desktop.h"
#include "Explorer.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "UIEditor.h"

void Desktop::Render()
{
	RenderMainMenuBar();
}

void Desktop::RenderMainMenuBar()
{
	if (!ImGui::BeginMainMenuBar())
		return;

	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Explorer", nullptr, Explorer::GetInstance()))
		{
			if (Explorer::IsInstanced())
				Explorer::Destroy();
			else
				Explorer::Instantiate();
		}
		if (ImGui::MenuItem("Hierarchy", nullptr, Hierarchy::GetInstance()))
		{
			if (Hierarchy::IsInstanced())
				Hierarchy::Destroy();
			else
				Hierarchy::Instantiate();
		}
		if (ImGui::MenuItem("Inspector", nullptr, Inspector::GetInstance()))
		{
			if (Inspector::IsInstanced())
				Inspector::Destroy();
			else
				Inspector::Instantiate();
		}
		/*
		if (ImGui::MenuItem("UI Editor", nullptr, UIEditor::GetInstance()))
		{
			if (UIEditor::IsInstanced())
				UIEditor::Destroy();
			else
				UIEditor::Instantiate();
		}
		*/
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}
