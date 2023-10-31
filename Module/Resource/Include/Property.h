#pragma once

namespace Resource
{
	class Image;

	class Property
	{
	public:
		friend class ResourceManager;

		enum class Type : char
		{
			GROUP, INT, INT2, FLOAT, STRING, IMAGE
		};

		class Iterator
		{
		public:
			Iterator(const Property* const p, size_t index);
			~Iterator() = default;

			DLLEXPORT Iterator& operator++();
			DLLEXPORT Iterator& operator--();
			DLLEXPORT bool operator!=(const Iterator& it) const;
			DLLEXPORT std::pair<std::string, std::shared_ptr<Property>> operator*() const;

		private:
			const Property* const m_property;
			size_t m_childIndex;
		};

	public:
		Property();
		~Property();

		DLLEXPORT Iterator begin() const;
		DLLEXPORT Iterator end() const;

		Type GetType() const;
		std::shared_ptr<Property> Get(const std::string& path) const;
		int GetInt() const;
		INT2 GetInt2() const;
		float GetFloat() const;
		std::string GetString() const;
		std::shared_ptr<Image> GetImage() const;
		ID2D1Bitmap* GetD2DImage() const;
		ComPtr<ID3D12Resource> GetD3DImage() const;
		void SetD3DImage(const ComPtr<ID3D12Resource>& image);

	private:
		// ResourceManager에서 사용하는 함수들
		void Load(std::ifstream& file, std::string& name);
		void Flush();

	private:
		Type m_type;
		std::string m_name;
		union
		{
			int m_int;
			INT2 m_int2;
			float m_float;
			std::string m_string;
			std::shared_ptr<Image> m_image;
		};
		std::vector<std::shared_ptr<Property>> m_children;
	};
}