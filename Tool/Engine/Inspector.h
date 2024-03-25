#pragma once
#include "Common/Singleton.h"

class Node;

class Inspector : public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaTime);
	void Render();

	Node* GetNode() const;

private:
	bool OnNodeDelete(Node* node);
	bool OnNodeSelect(Node* node);

	void RenderBasicInfo();

private:
	static constexpr auto WINDOW_NAME{ "Inspector" };
	static constexpr auto STRING_LENGTH_MAX{ 20ui64 };
	static constexpr auto PROPERTY_TYPES = std::array{ "FOLDER", "INT", "INT2", "FLOAT", "STRING", "IMAGE" };

	// 하이라키 윈도우에서 선택된 노드
	Node* m_node;

	// 입력받을 때 사용할 변수
	std::string m_name;
	int32_t m_int;
	INT2 m_int2;
	float m_float;
	std::string m_string;
};