#pragma once
#include "Stdafx.h"
#include "GameApp.h"

template <class T>
class ConstantBuffer
{
public:
	ConstantBuffer() : 
		m_data{ nullptr }
	{ }

	~ConstantBuffer()
	{
		if (m_buffer)
			m_buffer->Unmap(0, nullptr);
	}

	void Init()
	{
		auto device{ GameApp::GetInstance()->GetD3DDevice() };

		const UINT size{ (sizeof(T) + 255) & ~255 };
		CD3DX12_HEAP_PROPERTIES prop{ D3D12_HEAP_TYPE_UPLOAD };
		CD3DX12_RESOURCE_DESC desc{ CD3DX12_RESOURCE_DESC::Buffer(size) };

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL,
			IID_PPV_ARGS(&m_buffer)));

		DX::ThrowIfFailed(m_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_data)));
	}

	BOOL IsValid() const
	{
		return m_data ? TRUE : FALSE;
	}

	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, UINT rootParameterIndex) const
	{
		commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, m_buffer->GetGPUVirtualAddress());
	}

	T* operator->()
	{
		return m_data;
	}

private:
	ComPtr<ID3D12Resource> m_buffer;
	T* m_data;
};