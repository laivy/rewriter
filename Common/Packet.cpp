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

Packet::Packet(const char* buffer, Size size) :
	m_offset{ 0 }
{
	// 패킷 사이즈만큼 버퍼를 생성
	std::memcpy(&m_encodedSize, buffer, sizeof(m_encodedSize));
	m_buffer.reset(new char[m_encodedSize]{});
	m_bufferSize = m_encodedSize;

	// 타입
	std::memcpy(&m_type, buffer + sizeof(m_bufferSize), sizeof(m_type));

	// 데이터 복사
	EncodeBuffer(buffer + sizeof(m_bufferSize) + sizeof(m_type), size - sizeof(m_bufferSize) - sizeof(m_type));
}

void Packet::EncodeBuffer(const char* buffer, Size size)
{
	if (m_encodedSize + size > m_bufferSize)
		ReAlloc(size);

	std::memcpy(m_buffer.get() + m_offset, buffer, size);
	m_offset += size;
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
	while (bufferSize < m_encodedSize + requireSize);

	std::unique_ptr<char[]> buffer{ new char[bufferSize]{} };
	std::memcpy(buffer.get(), m_buffer.get(), m_bufferSize);
	m_buffer.swap(buffer);
	m_bufferSize = bufferSize;
}
