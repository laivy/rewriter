#pragma once

class Inspector : public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaTime);
	void Render();

	std::weak_ptr<Resource::Property> GetNode() const;

private:
	void OnPropertyDelete(std::shared_ptr<Resource::Property> prop);
	void OnPropertySelect(std::shared_ptr<Resource::Property> prop);

	void RenderBasicInfo();

private:
	static constexpr auto WINDOW_NAME{ "Inspector" };
	static constexpr auto STRING_LENGTH_MAX{ 30ui64 };

	// 옵저버
	std::unique_ptr<Observer<std::shared_ptr<Resource::Property>>> m_onPropertyDelete;
	std::unique_ptr<Observer<std::shared_ptr<Resource::Property>>> m_onPropertySelect;

	// 하이라키 윈도우에서 선택된 프로퍼티
	std::weak_ptr<Resource::Property> m_prop;
};