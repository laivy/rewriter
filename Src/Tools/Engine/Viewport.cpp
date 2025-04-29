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
	static std::shared_ptr<Resource::Model> robot;
	static std::shared_ptr<Graphics::D3D::RenderTarget> renderTarget;
	static std::shared_ptr<Graphics::D3D::Camera> camera;
	if (!robot)
	{
		if (auto prop{ Resource::Get(L"Data/Test.dat/Robot") })
			robot = prop->GetModel();
	}
	if (!renderTarget)
		renderTarget = Graphics::D3D::CreateRenderTarget(800, 600);
	if (!camera)
	{
		camera = Graphics::D3D::CreateCamera();
		camera->SetPosition(Int2{ 100, 100 });
	}

	if (ImGui::Begin(WINDOW_NAME))
	{
		//Graphics::D3D::PushRenderTarget(renderTarget);
		//Graphics::D3D::SetCamera(camera);
		//Graphics::D3D::Render(robot);
		//Graphics::D3D::PopRenderTarget();
		//Graphics::ImGui::Image(renderTarget, ImVec2{ 800.0f, 600.0f });
	}
	ImGui::End();
}
