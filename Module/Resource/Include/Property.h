#pragma once

namespace Resource
{
	class Property
	{
	public:
		friend class ResourceManager;

		enum class Type : char
		{
			GROUP, INT, INT2, FLOAT, STRING, IMAGE
		};

	private:
		class Iterator
		{
		public:
			Iterator(const Property* const p, size_t index);
			~Iterator() = default;

			Iterator& operator++();
			Iterator& operator--();
			bool operator!=(const Iterator& it) const;
			std::pair<std::string, std::shared_ptr<Property>> operator*() const;

		private:
			const Property* const m_property;
			size_t m_childIndex;
		};

	public:
		Property();
		~Property();

		Iterator begin() const;
		Iterator end() const;

		Type GetType() const;
		std::shared_ptr<Property> Get(const std::string& path) const;
		int GetInt() const;
		INT2 GetInt2() const;
		float GetFloat() const;
		std::string GetString() const;

	private:
		void Load(std::ifstream& file, std::string& name);

	public:
		Type m_type;
		std::string m_name;
		union
		{
			int m_int;
			INT2 m_int2;
			float m_float;
			std::string m_string;
			std::vector<char> m_binary;
		};
		std::vector<std::shared_ptr<Property>> m_children;
	};
}