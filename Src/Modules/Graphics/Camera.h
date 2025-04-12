#pragma once

namespace Graphics::D3D
{
	class Camera
	{
	public:
		DLL_API Camera();
		DLL_API ~Camera();

		DLL_API void SetPosition(const Int2& position);
		DLL_API void SetScale(float scale);

		DLL_API Int2 GetPosition() const;
		DLL_API float GetScale() const;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
