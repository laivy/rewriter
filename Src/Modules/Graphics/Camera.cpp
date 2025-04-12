#include "Stdafx.h"
#include "Camera.h"
#include "ConstantBuffer.h"

namespace Graphics::D3D
{
	class Camera::Impl
	{
	private:
		struct Constants
		{
			Int2 position;
			float scale;
			float _;
		};

	public:
		Impl() :
			m_constants{}
		{
			m_constants->position = {};
			m_constants->scale = 1.0f;
		}

		~Impl() = default;

		void SetPosition(const Int2& position)
		{
			m_constants->position = position;
		}

		void SetScale(float scale)
		{
			m_constants->scale = scale;
		}

		Int2 GetPosition() const
		{
			return m_constants->position;
		}

		float GetScale() const
		{
			return m_constants->scale;
		}

	private:
		ConstantBuffer<RootParameterIndex::Camera, Constants> m_constants;
	};

	DLL_API Camera::Camera() :
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
}
