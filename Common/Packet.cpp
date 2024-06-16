#include "Packet.h"

Packet::Packet(Packet::Type type) :
	m_type{ type },
	m_buffer{ new char[DEFAULT_BUFFER_SIZE]{} },
	m_bufferSize{ DEFAULT_BUFFER_SIZE },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	Encode<size_type>(-1);
	Encode<Type>(type);
}

Packet::Packet(const char* buffer, size_type size) :
	m_buffer{ nullptr },
	m_bufferSize{ 0 },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	// 패킷 사이즈만큼 버퍼를 생성
	std::memcpy(&m_bufferSize, buffer, sizeof(m_bufferSize));
	m_buffer.reset(new char[m_bufferSize]{});

	// 타입
	std::memcpy(&m_type, buffer + sizeof(m_bufferSize), sizeof(m_type));

	// 데이터 복사
	EncodeBuffer(buffer + sizeof(m_bufferSize) + sizeof(m_type), size - sizeof(m_bufferSize) - sizeof(m_type));
}

void Packet::EncodeBuffer(const char* buffer, size_type size)
{
	if (m_encodedSize + size > m_bufferSize)
		ReAlloc(size);

	std::memcpy(m_buffer.get() + m_offset, buffer, size);
	m_offset += size;
	m_encodedSize = std::max<size_type>(m_encodedSize, m_offset);
}

void Packet::End()
{
	// 패킷 크기를 0 위치에 씀
	EncodeAt(GetSize(), 0);
}

void Packet::SetOffset(size_type offset)
{
	m_offset = offset;
}

Packet::Type Packet::GetType() const
{
	return m_type;
}

const char* Packet::GetBuffer() const
{
	return m_buffer.get();
}

Packet::size_type Packet::GetSize() const
{
	return m_encodedSize;
}

void Packet::ReAlloc(size_type requireSize)
{
	size_type bufferSize{ m_bufferSize };
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