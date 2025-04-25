#include "Stdafx.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Global.h"

namespace Graphics::D3D
{
	class Camera::Impl
	{
	private:
		static constexpr auto Z{ -100.0f };
		static constexpr auto NearZ{ 0.1f };
		static constexpr auto FarZ{ 100.0f };

		struct Constants
		{
			Matrix view;
			Matrix proj;
		};

	public:
		Impl() :
			m_constants{},
			m_position{},
			m_scale{ 1.0f }
		{
		}

		~Impl() = default;

		void SetPosition(const Int2& position)
		{
			m_position = position;
		}

		void SetScale(float scale)
		{
			m_scale = scale;
		}

		Int2 GetPosition() const
		{
			return m_position;
		}

		float GetScale() const
		{
			return m_scale;
		}

		void SetShaderConstants() const
		{
			auto eye{ DirectX::XMVectorSet(static_cast<float>(m_position.x), static_cast<float>(m_position.y), Z, 0.0f) };
			auto at{ DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) };
			auto up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

			// 뷰 변환 행렬
			DirectX::XMFLOAT4X4 view{};
			DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(eye, at, up)));
			std::memcpy(&m_constants->view, &view, sizeof(view));

			// 투영 변환 행렬
			DirectX::XMFLOAT4X4 proj{};
			DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(g_renderTargetSize.x), static_cast<float>(g_renderTargetSize.y), NearZ, FarZ)));
			std::memcpy(&m_constants->proj, &proj, sizeof(proj));

			g_commandList->SetGraphicsRootConstantBufferView(m_constants.GetRootParameterIndex(), m_constants.GetGPUVirtualAddress());
		}

	private:
		ConstantBuffer<RootParameterIndex::Camera, Constants> m_constants;
		Int2 m_position;
		float m_scale;
	};

	Camera::Camera() :
		m_impl{ new Camera::Impl{} }
	{
	}

	DLL_API Camera::~Camera()
	{
	}

	DLL_API void Camera::SetPosition(const Int2& position)
	{
		m_impl->SetPosition(position);
	}

	DLL_API void Camera::SetScale(float scale)
	{
		m_impl->SetScale(scale);
	}

	DLL_API Int2 Camera::GetPosition() const
	{
		return m_impl->GetPosition();
	}

	DLL_API float Camera::GetScale() const
	{
		return m_impl->GetScale();
	}

	void Camera::SetShaderConstants() const
	{
		m_impl->SetShaderConstants();
	}
}
