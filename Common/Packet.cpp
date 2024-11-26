#include "Stdafx.h"
#include "Packet.h"

Packet::Packet(Protocol type) :
	m_type{ type },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	m_buffer.resize(DEFAULT_BUFFER_SIZE);
	Encode<Size>(0);
	Encode<Protocol>(type);
}

// 서버를 통해 수신한 데이터로부터 패킷을 조립하는 생성자
Packet::Packet(std::span<char> buffer) :
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	Size packetSize{};
	std::memcpy(&packetSize, buffer.data(), sizeof(packetSize));
	m_buffer.resize(packetSize);

	EncodeBuffer(buffer);

	// 버퍼 데이터로부터 타입 세팅해줌
	std::memcpy(&m_type, m_buffer.data() + sizeof(m_encodedSize), sizeof(m_type));
}

Packet& Packet::operator=(Packet&& rhs) noexcept
{
	m_type = rhs.m_type;
	m_buffer.swap(rhs.m_buffer);
	m_encodedSize = rhs.m_encodedSize;
	m_offset = rhs.m_offset;

	rhs.m_type = Protocol::None;
	std::vector<char>{}.swap(rhs.m_buffer);
	rhs.m_encodedSize = 0;
	rhs.m_offset = 0;
	return *this;
}

void Packet::SetOffset(Size offset)
{
	m_offset = offset;
}

void Packet::Reset()
{
	m_type = Protocol::None;
	std::vector<char>(DEFAULT_BUFFER_SIZE).swap(m_buffer);
	m_encodedSize = 0;
	m_offset = 0;
}

Protocol Packet::GetType() const
{
	return m_type;
}

char* Packet::GetBuffer()
{
	return m_buffer.data();
}

Packet::Size Packet::GetSize() const
{
	return m_encodedSize;
}

void Packet::EncodeBuffer(std::span<const char> buffer)
{
	if (m_offset + buffer.size() > m_buffer.size())
		ReAlloc(static_cast<Size>(m_offset + buffer.size() - m_buffer.size()));

	std::memcpy(m_buffer.data() + m_offset, buffer.data(), buffer.size());
	m_offset += static_cast<Size>(buffer.size());
	m_encodedSize = std::max<Size>(m_encodedSize, m_offset);

	// 패킷 크기 갱신
	std::memcpy(m_buffer.data(), &m_encodedSize, sizeof(m_encodedSize));
}

void Packet::ReAlloc(Size requireSize)
{
	Size bufferSize{ static_cast<Size>(m_buffer.size()) };
	do
	{
		bufferSize *= 2;
	}
	while (bufferSize < m_buffer.size() + requireSize);
	m_buffer.resize(bufferSize);
}
