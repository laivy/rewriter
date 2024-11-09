#pragma once

class Packet
{
public:
	using Size = uint16_t; // 패킷 크기 타입
	enum class Type : uint16_t
	{
		None,
		ServerBasicInfo,

		// 회원가입
		RequestRegister,
		RequestRegisterToCenter,
		RegisterResultToLogin,
		RegisterResult,

		// 로그인
		RequestLogin,
		RequestLoginToCenter,
		LoginResultToLogin,
		LoginResult
	};

public:
	Packet(Type type);
	Packet(std::span<char> buffer);
	~Packet() = default;

	Packet& operator=(Packet&& rhs) noexcept;

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
		_Decode(data, std::index_sequence_for<Args...>{});
		return data;
	}

	void SetOffset(Size offset);

	Type GetType() const;
	char* GetBuffer();
	Size GetSize() const;

private:
	void EncodeBuffer(std::span<const char> buffer);

	// 버퍼에 sizeof(T) 만큼 씀
	template<class T>
	void _Encode(const T& data)
	{
		if (m_offset + sizeof(T) > static_cast<Size>(m_buffer.size()))
			ReAlloc(m_offset + sizeof(T) - static_cast<Size>(m_buffer.size()));

		std::memcpy(m_buffer.data() + m_offset, &data, sizeof(T));
		m_offset += sizeof(T);
		m_encodedSize = std::max<Size>(m_encodedSize, m_offset);

		// 패킷 크기 갱신
		std::memcpy(m_buffer.data(), &m_encodedSize, sizeof(m_encodedSize));
	}

	template<class T>
	requires requires(const T& t)
	{
		typename T::value_type;
		{ std::size(t) } -> std::same_as<std::size_t>;
		{ std::data(t) };
	}
	void _Encode(const T& data)
	{
		_Encode(static_cast<unsigned int>(std::size(data)));
		EncodeBuffer(std::span{ reinterpret_cast<const char*>(std::data(data)), std::size(data) * sizeof(T::value_type) });
	}

	template<class T, size_t N>
	void _Encode(const T (&data)[N])
	{
		_Encode(static_cast<unsigned int>(N));
		EncodeBuffer(std::span{ reinterpret_cast<const char*>(&data), N * sizeof(T) });
	}

	template<class T>
	void EncodeAt(const T& data, Size offset)
	{
		auto temp{ m_offset };
		SetOffset(offset);
		_Encode(data);
		SetOffset(temp);
	}

	// 버퍼에서 sizeof(T) 만큼 읽어서 T로 반환
	template<class T>
	T _Decode()
	{
		T data{};
		std::memcpy(&data, m_buffer.data() + m_offset, sizeof(T));
		m_offset += sizeof(T);
		return data;
	}

	template<class T>
	requires requires(T t)
	{
		typename T::value_type;
		{ std::size(t) } -> std::same_as<std::size_t>;
		{ std::data(t) };
	}
	T _Decode()
	{
		auto size{ _Decode<unsigned int>() };
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>)
		{
			T string(size, '\0');
			std::memcpy(string.data(), m_buffer.data() + m_offset, size * sizeof(T::value_type));
			m_offset += static_cast<Size>(size * sizeof(T::value_type));
			return string;
		}
		else if constexpr (std::is_same_v<T, std::vector<T::value_type>>)
		{
			T vector(size);
			std::memcpy(vector.data(), m_buffer.data() + m_offset, size * sizeof(T::value_type));
			m_offset += static_cast<Size>(size * sizeof(T::value_type));
			return vector;
		}
		else
		{
			T data{};
			std::memcpy(&data, m_buffer.data() + m_offset, size * sizeof(T::value_type));
			m_offset += static_cast<Size>(size * sizeof(T::value_type));
			return data;
		}
	}

	template<class... Args, size_t... Indices>
	void _Decode(std::tuple<Args...>& data, std::index_sequence<Indices...>)
	{
		((std::get<Indices>(data) = _Decode<std::remove_reference_t<decltype(std::get<Indices>(data))>>()), ...);
	}

	void ReAlloc(Size requireSize);

private:
	static constexpr Size DEFAULT_BUFFER_SIZE{ 128 };

	Type m_type;
	std::vector<char> m_buffer;
	Size m_encodedSize;
	Size m_offset;
};
