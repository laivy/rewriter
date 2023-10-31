#include "Packet.h"

Packet::Packet(Packet::Type type) :
	m_type{ type },
	m_buffer{ new char[DEFAULT_BUFFER_SIZE] },
	m_bufferSize{ DEFAULT_BUFFER_SIZE },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	// 패킷 처음의 4바이트(int)는 패킷 크기를 쓸 예정
	EncodeAt(sizeof(int), type);
}

Packet::Packet(const char* buffer, int size) :
	m_buffer{ new char[size] },
	m_bufferSize{ size },
	m_encodedSize{ size },
	m_offset{ 0 }
{
	std::memmove(&m_type, buffer + sizeof(int), sizeof(m_type));
	std::memmove(m_buffer.get(),
		buffer + sizeof(int) + sizeof(m_type),
		size - sizeof(int) - sizeof(m_type));
}

void Packet::EncodeBuffer(const char* buffer, int size)
{
	if (m_bufferSize < m_offset + size)
		ReAlloc();

	std::memcpy(m_buffer.get() + m_offset, buffer, size);
	m_encodedSize = std::max(m_encodedSize, m_offset + size);
	m_offset += size;
}

void Packet::End()
{
	// 패킷 크기를 0 위치에 씀
	EncodeAt(0, GetSize());
}

Packet::Type Packet::GetType() const
{
	return m_type;
}

const char* Packet::GetBuffer() const
{
	return m_buffer.get();
}

int Packet::GetSize() const
{
	return m_encodedSize;
}

void Packet::ReAlloc()
{
	int bufferSize{ m_bufferSize + m_bufferSize / 2 };
	std::unique_ptr<char[]> buffer{ new char[bufferSize] };
	std::memmove(buffer.get(), m_buffer.get(), m_bufferSize);
	m_buffer.swap(buffer);
	m_bufferSize = bufferSize;
}
