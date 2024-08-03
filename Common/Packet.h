#pragma once

class Packet
{
public:
	using Size = unsigned short; // 패킷 크기
	enum class Type : unsigned short // 패킷 타입
	{
		None,

		// 회원가입
		RequestRegister,
		RequestRegisterToCenter,
		RegisterCenterResult,
		RegisterResult,

		// 로그인
		RequestLoginIn,
		LoginResult,
	};

public:
	Packet(Type type = Type::None);
	Packet(const char* buffer, Size size);
	~Packet() = default;

	template<class T, class... Args>
	void Encode(const T& arg, const Args&... args)
	{
		_Encode(arg);
		if constexpr (sizeof...(Args) > 0)
			Encode(args...);
	}

	template<class Arg>
	Arg Decode()
	{
		return _Decode<Arg>();
	}

	template<class... Args>
	requires (sizeof...(Args) > 1)
	std::tuple<Args...> Decode()
	{
		std::tuple<Args...> data{};
		_Decode<0>(data);
		return data;
	}

	void EncodeBuffer(const char* buffer, Size size);
	void End();

	char* Detach();

	void SetOffset(Size offset);

	Type GetType() const;
	char* GetBuffer() const;
	Size GetSize() const;

private:
	template<class T>
	void EncodeAt(const T& data, unsigned int offset)
	{
		auto temp{ m_offset };
		SetOffset(offset);
		_Encode(data);
		SetOffset(temp);
	}

	// 버퍼에 sizeof(T) 만큼 씀
	template<class T>
	void _Encode(const T& data)
	{
		if (m_encodedSize + sizeof(T) > m_bufferSize)
			ReAlloc(sizeof(T));

		std::memcpy(m_buffer.get() + m_offset, &data, sizeof(T));
		m_offset += sizeof(T);
		m_encodedSize = std::max<unsigned int>(m_encodedSize, m_offset);
	}

	// 배열은 원소 개수 + 데이터를 씀
	template<class T>
	requires std::is_array_v<T>
	void _Encode(const T& data)
	{
		_Encode(data.size());
		EncodeBuffer(data.data(), sizeof(T));
	}

	// 문자열은 글자 개수 + 데이터를 씀
	template<class T>
	requires std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>
	void _Encode(const T& data)
	{
		_Encode(static_cast<unsigned int>(data.size()));
		EncodeBuffer(reinterpret_cast<const char*>(data.data()), static_cast<Packet::Size>(data.size() * sizeof(T::value_type)));
	}

	// 버퍼에서 sizeof(T) 만큼 읽어서 T로 반환
	template<class T>
	T _Decode()
	{
		T data{};
		std::memcpy(&data, m_buffer.get() + m_offset, sizeof(T));
		m_offset += sizeof(T);
		return data;
	}

	// 튜플의 N번째 데이터를 디코드하고 N+1번째 데이터 디코드
	template<int N, class... Args>
	void _Decode(std::tuple<Args...>& data)
	{
		if constexpr (N < sizeof...(Args))
		{
			std::get<N>(data) = _Decode<std::remove_reference_t<decltype(std::get<N>(data))>>();
			_Decode<N + 1>(data);
		}
	}

	template<class T>
	requires std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>
	T _Decode()
	{
		auto length{ _Decode<unsigned int>() };
		T value{ reinterpret_cast<T::value_type*>(m_buffer.get() + m_offset), static_cast<size_t>(length) };
		m_offset += static_cast<Packet::Size>(length * sizeof(T::value_type));
		return value;
	}

	void ReAlloc(Size requireSize);

private:
	static constexpr Size DEFAULT_BUFFER_SIZE{ 128 };

	Type m_type;
	std::unique_ptr<char[]> m_buffer;
	Size m_bufferSize;
	Size m_encodedSize;
	Size m_offset;
};