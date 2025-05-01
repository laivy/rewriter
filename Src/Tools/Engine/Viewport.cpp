#include "Stdafx.h"
#include "Viewport.h"


Viewport::Viewport()
{
}

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
