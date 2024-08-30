#include "Stdafx.h"
#include "UIEditor.h"

void UIEditor::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
	}
	ImGui::End();
	ImGui::PopID();
}
