#pragma once

class Node
{
public:
	Node();
	virtual ~Node() = default;

	std::vector<std::unique_ptr<Node>>::iterator begin();
	std::vector<std::unique_ptr<Node>>::iterator end();

	void Render();

	void Add(std::unique_ptr<Node> child);
	void Delete();
	void DeleteInvalidChildren();

	template<class T>
	requires is_property_data_type_v<T>
	void Set(const T& value)
	{
		if (m_property)
			m_property->Set(value);
	}

	void SetProperty(const std::shared_ptr<Resource::Property>& prop);
	void SetType(Resource::Property::Type type);
	void SetName(const std::string& name);
	void SetParent(Node* parent);
	void SetFilePath(const std::filesystem::path& path);
	void SetSelect(bool isSelect);

	bool IsRoot() const;
	Resource::Property::Type GetType() const;
	std::string GetName() const;
	int GetInt() const;
	INT2 GetInt2() const;
	float GetFloat() const;
	std::string GetString() const;
	std::shared_ptr<Resource::Image> GetImage() const;
	Node* GetParent() const;
	std::filesystem::path GetFilePath() const;
	bool IsSelected() const;
	bool IsValid() const;

private:
	void RenderContextMenu();

private:
	static inline size_t s_index{ 0 };

protected:
	static constexpr auto DEFAULT_NODE_NAME{ "Node" };

	size_t m_id;
	bool m_isValid;
	bool m_isSelected;
	Node* m_parent;
	std::shared_ptr<Resource::Property> m_property;
	std::vector<std::unique_ptr<Node>> m_children;
	std::filesystem::path m_filePath;
};