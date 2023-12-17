#pragma once

class Node
{
public:
	Node();
	virtual ~Node() = default;

	virtual void Render();
	virtual bool IsRootNode() const;

	std::vector<std::unique_ptr<Node>>::iterator begin();
	std::vector<std::unique_ptr<Node>>::iterator end();

	// 노드가 선택됐을 때 호출됨
	// 다른 윈도우들로 이벤트 전달
	void OnNodeSelected();

	template<class T>
	requires is_property_data_type_v<T>
	void Set(const T& data)
	{
		if (m_property)
			Resource::Set(m_property, data);
	}

	void SetParent(Node* parent);
	void SetType(Resource::Property::Type type);
	void SetName(const std::string& name);
	void SetSelect(bool isSelect);

	Resource::Property::Type GetType() const;
	std::string GetName() const;
	int GetInt() const;
	INT2 GetInt2() const;
	float GetFloat() const;
	std::string GetString() const;
	std::shared_ptr<Resource::Image> GetImage() const;
	Node* GetParent() const;
	bool IsSelected() const;

private:
	void RenderContextMenu();

protected:
	static constexpr auto DEFAULT_NODE_NAME{ "NewProperty" };

	size_t m_id;
	Node* m_parent;
	std::vector<std::unique_ptr<Node>> m_children;

	std::shared_ptr<Resource::Property> m_property;
	bool m_isSelected;

private:
	static inline size_t s_index{ 0 };
};

class RootNode : public Node
{
public:
	RootNode() = default;
	virtual ~RootNode() override final = default;

	virtual void Render() override final;
	virtual bool IsRootNode() const override final;

	void Save() const;

private:
	void RenderContextMenu();

private:
	std::filesystem::path m_filePath;
};