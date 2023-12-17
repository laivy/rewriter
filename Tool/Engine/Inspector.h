#pragma once
#include "Common/Singleton.h"

class Node;

class Inspector : public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Render();

	// 하이라키 윈도우에서 노드가 선택될 때 선택된 노드를 전달받는다.
	void OnNodeSelected(Node* node);

	Node* GetNode() const;

private:
	void RenderBasicInfo();

private:
	static constexpr auto WINDOW_NAME{ "Inspector" };
	static constexpr auto STRING_LENGTH_MAX{ 20 };
	static constexpr auto PROPERTY_TYPES = std::array<const char*, 6>{ "FOLDER", "INT", "INT2", "FLOAT", "STRING", "IMAGE" };

	// 하이라키 윈도우에서 선택된 노드
	Node* m_node;

	// 입력받을 때 사용할 변수
	std::string m_name;
	int32_t m_int;
	std::array<int, 2> m_int2;
	float m_float;
	std::string m_string;
};