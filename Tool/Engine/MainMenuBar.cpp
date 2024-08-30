#include "Stdafx.h"
#include "MainMenuBar.h"
#include "UIEditor.h"

void MainMenuBar::Render()
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
