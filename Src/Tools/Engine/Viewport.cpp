#include "Stdafx.h"
#include "Viewport.h"

std::shared_ptr<Graphics::ImGui::Texture> g_texture;

Viewport::Viewport()
{
	if (!g_texture)
		g_texture = Graphics::ImGui::LoadTexture(L"Engine/Cream.png");
}

void Viewport::Update(float deltaTime)
{
}

void Viewport::Render()
{
	if (ImGui::Begin(WINDOW_NAME))
	{
		Graphics::ImGui::Image(g_texture);
	}
	ImGui::End();
}
