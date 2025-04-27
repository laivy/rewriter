#include "Stdafx.h"
#include "Viewport.h"

void Viewport::Update(float deltaTime)
{
}

void Viewport::Render()
{
	if (ImGui::Begin(WINDOW_NAME))
	{
	}
	ImGui::End();
}
