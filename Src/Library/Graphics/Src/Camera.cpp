#include "Pch.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Global.h"
#include "SwapChain.h"

namespace
{
	constexpr auto Z{ -100.0f };
	constexpr auto NearZ{ 0.1f };
	constexpr auto FarZ{ 100.0f };

	std::shared_ptr<Graphics::D3D::Camera> g_camera;
}

namespace Graphics::D3D
{
	struct Camera
	{
		struct Constants
		{
			Matrix view;
			Matrix proj;
		};

		ConstantBuffer<RootParameterIndex::Camera, Constants> constants;

		Int2 position;
		float scale;
	};

	std::shared_ptr<Camera> NewCamera()
	{
		return std::make_shared<Camera>();
	}

	std::shared_ptr<Camera> GetCamera()
	{
		return g_camera;
	}

	void SetCamera(const std::shared_ptr<Camera>& camera)
	{
		g_camera = camera;
		SetCameraShaderConstants(camera);
	}

	void SetCameraPosition(const std::shared_ptr<Camera>& camera, Int2 position)
	{
		camera->position = position;
	}

	void SetCameraScale(const std::shared_ptr<Camera>& camera, float scale)
	{
		camera->scale = scale;
	}

	void SetCameraShaderConstants(const std::shared_ptr<Camera>& camera)
	{
		auto eye{ DirectX::XMVectorSet(static_cast<float>(camera->position.x), static_cast<float>(camera->position.y), Z, 0.0f) };
		auto at{ DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) };
		auto up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

		// 뷰 변환 행렬
		DirectX::XMFLOAT4X4 view{};
		DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(eye, at, up)));
		std::memcpy(&camera->constants->view, &view, sizeof(view));

		// 투영 변환 행렬
		DirectX::XMFLOAT4X4 proj{};
		Int2 size{ g_swapChain->GetSize() };
		DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(size.x), static_cast<float>(size.y), NearZ, FarZ)));
		std::memcpy(&camera->constants->proj, &proj, sizeof(proj));

		g_commandList->SetGraphicsRootConstantBufferView(camera->constants.GetRootParameterIndex(), camera->constants.GetGPUVirtualAddress());
	}
}
