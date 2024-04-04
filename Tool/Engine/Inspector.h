#pragma once
#include "Common/Singleton.h"

struct PropInfo;

class Inspector : public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaTime);
	void Render();

	std::shared_ptr<Resource::Property> GetNode() const;

private:
	void OnNodeDelete(std::shared_ptr<Resource::Property> prop);
	void OnNodeSelect(std::shared_ptr<Resource::Property> prop);

	void RenderBasicInfo();

private:
	static constexpr auto WINDOW_NAME{ "Inspector" };
	static constexpr auto STRING_LENGTH_MAX{ 20ui64 };
	static constexpr auto PROPERTY_TYPES = std::array{ "FOLDER", "INT", "INT2", "FLOAT", "STRING", "IMAGE" };

	// 옵저버
	Observer<std::shared_ptr<Resource::Property>> m_onNodeDelete;
	Observer<std::shared_ptr<Resource::Property>> m_onNodeSelect;

	// 하이라키 윈도우에서 선택된 프로퍼티
	std::shared_ptr<Resource::Property> m_prop;

	// 입력받을 때 사용할 변수
	std::string m_name;
	int32_t m_int;
	INT2 m_int2;
	float m_float;
	std::string m_string;
};