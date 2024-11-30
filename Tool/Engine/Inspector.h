#pragma once

class Inspector :
	public IObserver,
	public TSingleton<Inspector>
{
public:
	Inspector();
	~Inspector() = default;

	void Update(float deltaTime);
	void Render();

private:
	void OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertySelected(const std::shared_ptr<Resource::Property>& prop);

	void RenderNode();
	void RenderNodeName(const std::shared_ptr<Resource::Property>& prop);
	void RenderNodeType(const std::shared_ptr<Resource::Property>& prop);
	void RenderNodeValue(const std::shared_ptr<Resource::Property>& prop);

private:
	static constexpr auto WINDOW_NAME{ "Inspector" };
	static constexpr auto STRING_LENGTH_MAX{ 30ui64 };

	// 하이라키 윈도우에서 가장 마지막으로 선택된 프로퍼티
	std::weak_ptr<Resource::Property> m_prop;
};
