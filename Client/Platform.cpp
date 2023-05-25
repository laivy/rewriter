#include "Stdafx.h"
#include "Platform.h"

Platform::Platform(INT2 startPosition, INT2 endPosition) :
	m_startPosition{ startPosition },
	m_endPosition{ endPosition }
{

}

void Platform::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{

}
