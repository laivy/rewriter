#pragma once
#include <WinSock2.h>
#include <memory>
#include <string>

class Packet
{
public:
	enum class Type : int
	{
		CLIENT_TryLogin,
		LOGIN_TryLogin,
	};

public:
	Packet(Type type);
	Packet(const char* buffer, int size);
	~Packet() = default;

	template<class T, class... Args>
	void Encode(const T& arg, const Args&... args)
	{
		Encode(arg);
		Encode(args...);
	}

	template<class T>
	void Encode(const T& arg)
	{
		// 현재 위치에 쓰려는데 버퍼가 모자랄 경우 재할당
		if (m_bufferSize < m_offset + sizeof(arg))
			ReAlloc();

		std::memcpy(m_buffer.get() + m_offset, reinterpret_cast<const void*>(&arg), sizeof(arg));
		m_encodedSize = std::max(m_encodedSize, m_offset + static_cast<int>(sizeof(arg)));
		m_offset += sizeof(arg);
	}

	template<>
	void Encode(const std::string& arg)
	{
		Encode(static_cast<int>(arg.size()));
		EncodeBuffer(arg.data(), static_cast<int>(arg.size()));
	}

	void EncodeBuffer(const char* buffer, int size);

	// 버퍼의 지정한 오프셋 위치에 쓴다.
	// 쓰기 후엔 오프셋을 버퍼 끝으로 옮긴다.
	template<class T>
	void EncodeAt(int offset, const T& arg)
	{
		int oldOffset{ m_offset };
		m_offset = offset;
		Encode(arg);
		m_offset = m_encodedSize;
	}

	template<class... Args>
	std::tuple<Args...> Decode()
	{
		std::tuple<Args...> value{};
		_Decode<0>(value);
		return value;
	}

	Type GetType() const;
	const char* GetBuffer() const;
	int GetSize() const;

private:
	template<int N, class... Args>
	void _Decode(std::tuple<Args...>& value)
	{
		if constexpr (N < sizeof...(Args))
		{
			std::get<N>(value) = _Decode<std::remove_reference_t<decltype(std::get<N>(value))>>();
			_Decode<N + 1>(value);
		}
	}

	template<class T>
	T _Decode()
	{
		T value{};
		std::memmove(reinterpret_cast<void*>(&value), m_buffer.get() + m_offset, sizeof(T));
		m_offset += sizeof(T);
		return value;
	}

	template<>
	std::string _Decode()
	{
		int length{ _Decode<int>() };
		std::string value{ m_buffer.get() + m_offset, static_cast<size_t>(length) };
		m_offset += length;
		return value;
	}

	void ReAlloc();

private:
	static constexpr auto DEFAULT_BUFFER_SIZE = 128;

	Type m_type; // 패킷 타입
	std::unique_ptr<char[]> m_buffer;
	int m_bufferSize; // 버퍼 크기
	int m_encodedSize; // 버퍼에 쓰여진 크기
	int m_offset; // 버퍼에서 가르키고 있는 위치
};