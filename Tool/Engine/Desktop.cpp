#include "Stdafx.h"
#include "Desktop.h"
#include "UIEditor.h"

void Desktop::Render()
{
	RenderMainMenuBar();
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
