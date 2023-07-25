#include "Stdafx.h"
#include "Platform.h"

#ifdef _DEBUG
#include "ResourceManager.h"
#endif

Platform::Platform(INT2 startPosition, INT2 endPosition)
{
	// x값이 작은 것을 startPosition으로 설정한다.
	if (startPosition.x > endPosition.x)
	{
		m_startPosition = endPosition;
		m_endPosition = startPosition;
	}
	else
	{
		m_startPosition = startPosition;
		m_endPosition = endPosition;
	}

#ifdef _DEBUG
	m_cbLine.Init();
	m_cbLine->position1 = { static_cast<float>(m_startPosition.x), static_cast<float>(m_startPosition.y) };
	m_cbLine->position2 = { static_cast<float>(m_endPosition.x), static_cast<float>(m_endPosition.y) };
	m_cbLine->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::Type::DEFAULT);
	m_shader = rm->GetShader(Shader::Type::LINE);
#endif
}

void Platform::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
#ifdef _DEBUG
	if (m_cbLine.IsValid())
		m_cbLine.SetShaderVariable(commandList, RootParamIndex::LINE);
	if (auto s{ m_shader.lock() })
		commandList->SetPipelineState(s->GetPipelineState());
	if (auto m{ m_mesh.lock() })
		m->Render(commandList);
#endif
}

std::pair<INT2, INT2> Platform::GetStartEndPosition() const
{
	return std::make_pair(m_startPosition, m_endPosition);
}

// x에서의 플렛폼의 높이값을 반환한다.
float Platform::GetHeight(FLOAT x) const
{
	if (static_cast<FLOAT>(m_startPosition.x) > x ||
		static_cast<FLOAT>(m_endPosition.x) < x)
		return -FLT_MAX;

	if (m_startPosition.y == m_endPosition.y)
		return static_cast<float>(m_startPosition.y);

	return static_cast<float>(std::lerp(m_startPosition.y, m_endPosition.y, (x - m_startPosition.x) / (m_endPosition.x - m_startPosition.x)));
}

bool Platform::IsBetweenX(float x) const
{
	return static_cast<float>(m_startPosition.x) <= x && x <= static_cast<float>(m_endPosition.x);
}

bool Platform::IsBetweenY(float y) const
{
	float minY{ std::min(static_cast<float>(m_startPosition.y), static_cast<float>(m_endPosition.y)) };
	float maxY{ std::max(static_cast<float>(m_startPosition.y), static_cast<float>(m_endPosition.y)) };
	return minY <= y && y <= maxY;
}
