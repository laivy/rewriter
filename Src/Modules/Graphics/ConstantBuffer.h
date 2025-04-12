#pragma once
#include "Graphics3DUtil.h"

namespace Graphics::D3D
{
	enum class RootParameterIndex
	{
		Camera,
	};

	template<RootParameterIndex I, class T>
	requires std::is_standard_layout_v<T>
	class ConstantBuffer
	{
	public:
		ConstantBuffer()
		{
			// 버퍼 크기는 256바이트 정렬
			constexpr auto size{ (sizeof(T) + 255) & ~255 };
			m_buffer = CreateResourceBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size);
			m_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_data));
			new(m_data) T{};
		}

		T* operator->() const
		{
			return m_data;
		}

		UINT GetRootParameterIndex() const
		{
			return static_cast<UINT>(I);
		}

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
		{
			return m_buffer->GetGPUVirtualAddress();
		}

	private:
		ComPtr<ID3D12Resource> m_buffer;
		T* m_data;
	};
}
