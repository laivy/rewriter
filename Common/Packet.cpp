#include "Stdafx.h"
#include "Packet.h"

Packet::Packet(Packet::Type type) :
	m_type{ type },
	m_buffer{ new char[DEFAULT_BUFFER_SIZE]{} },
	m_bufferSize{ DEFAULT_BUFFER_SIZE },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	Encode<Size>(-1);
	Encode<Type>(type);
}

// 서버를 통해 수신한 데이터로부터 패킷을 조립하는 생성자
Packet::Packet(std::span<char> buffer) :
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	// 패킷 크기만큼 버퍼 생성
	Size packetSize{};
	std::memcpy(&packetSize, buffer.data(), sizeof(packetSize));

	m_buffer = std::make_unique<char[]>(packetSize);
	m_bufferSize = packetSize;

	EncodeBuffer(buffer);

	// 버퍼 데이터로부터 타입 세팅해줌
	std::memcpy(&m_type, m_buffer.get() + sizeof(m_encodedSize), sizeof(m_type));
}

void Packet::EncodeBuffer(std::span<const char> buffer)
{
	if (m_offset + buffer.size() > m_bufferSize)
		ReAlloc(static_cast<Size>(m_offset + buffer.size() - m_bufferSize));

	std::memcpy(m_buffer.get() + m_offset, buffer.data(), buffer.size());
	m_offset += static_cast<Size>(buffer.size());
	m_encodedSize = std::max<Size>(m_encodedSize, m_offset);

	// 패킷 크기 갱신
	std::memcpy(m_buffer.get(), &m_encodedSize, sizeof(m_encodedSize));
}

char* Packet::Detach()
{
	m_type = Type::None;
	m_bufferSize = 0;
	m_encodedSize = 0;
	m_offset = 0;
	return m_buffer.release();
}

void Packet::SetOffset(Size offset)
{
	m_offset = offset;
}

Packet::Type Packet::GetType() const
{
	return m_type;
}

char* Packet::GetBuffer() const
{
	return m_buffer.get();
}

Packet::Size Packet::GetSize() const
{
	return m_encodedSize;
}

void Packet::ReAlloc(Size requireSize)
{
	Size bufferSize{ m_bufferSize };
	do
	{
		bufferSize *= 2;
	}
	while (bufferSize < m_bufferSize + requireSize);

	std::unique_ptr<char[]> buffer{ new char[bufferSize]{} };
	std::memcpy(buffer.get(), m_buffer.get(), m_bufferSize);
	m_buffer.swap(buffer);
	m_bufferSize = bufferSize;
}
