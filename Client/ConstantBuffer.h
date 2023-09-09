#pragma once
#include "Stdafx.h"
#include "GameApp.h"

template <class T>
class ConstantBuffer
{
private:
	constexpr static UINT BUFFER_SIZE = (sizeof(T) + 255) & ~255;

public:
	ConstantBuffer() : 
		m_data{ nullptr }
	{
	}

	ConstantBuffer(const ConstantBuffer& rhs) :
		m_buffer{ rhs.m_buffer },
		m_data{ rhs.m_data }
	{		
	}

	~ConstantBuffer()
	{
		if (m_buffer)
			m_buffer->Unmap(0, nullptr);
	}

	void Init()
	{
		auto device{ GameApp::GetInstance()->GetD3DDevice() };
		DX::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD },
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL,
			IID_PPV_ARGS(&m_buffer)));
		m_buffer->SetName(TEXT("CONSTANT BUFFER"));
		DX::ThrowIfFailed(m_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_data)));

		// 기본 생성자 호출
		*m_data = {};
	}

	bool IsValid() const
	{
		return m_data ? true : false;
	}

	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex) const
	{
		commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(rootParameterIndex), m_buffer->GetGPUVirtualAddress());
	}

	T* operator->()
	{
		return m_data;
	}

private:
	ComPtr<ID3D12Resource> m_buffer;
	T* m_data;
};