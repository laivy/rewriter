#include "Packet.h"

Packet::Packet(Packet::Type type) :
	m_type{ type },
	m_buffer{ new char[DEFAULT_BUFFER_SIZE]{} },
	m_bufferSize{ DEFAULT_BUFFER_SIZE },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	Encode(-1);
	Encode(type);
}

Packet::Packet(const char* buffer) :
	m_buffer{ nullptr },
	m_bufferSize{ 0 },
	m_encodedSize{ 0 },
	m_offset{ 0 }
{
	std::memmove(&m_bufferSize, buffer, sizeof(int));
	m_buffer.reset(new char[m_bufferSize]{});

	std::memmove(&m_type, buffer + sizeof(m_bufferSize), sizeof(m_type));
	std::memmove(
		m_buffer.get(),
		buffer + sizeof(m_bufferSize) + sizeof(m_type),
		m_bufferSize - sizeof(m_bufferSize) - sizeof(m_type)
	);
}

void Packet::End()
{
	// 패킷 크기를 0 위치에 씀
	EncodeAt(GetSize(), 0);
}

Packet::Type Packet::GetType() const
{
	return m_type;
}

const char* Packet::GetBuffer() const
{
	return m_buffer.get();
}

size_t Packet::GetSize() const
{
	return m_encodedSize;
}

void Packet::EncodeBuffer(const char* buffer, size_t size)
{
	if (m_encodedSize + size > m_bufferSize)
		ReAlloc(size);

	std::memcpy(m_buffer.get() + m_offset, buffer, size);
	m_offset += size;
	m_encodedSize = std::max<unsigned int>(m_encodedSize, m_offset + size);
}

void Packet::ReAlloc(size_t requireSize)
{
	size_t bufferSize{ m_bufferSize };
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