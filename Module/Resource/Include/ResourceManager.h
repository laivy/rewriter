#pragma once

namespace Resource
{
	class Property;

	class ResourceManager : public TSingleton<ResourceManager>
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		std::shared_ptr<Property> Get(const std::string& fileName);

	private:
		std::shared_ptr<Property> Load(const std::string& fileName);

	private:
		std::unordered_map<std::string, std::shared_ptr<Property>> m_resources; // <파일이름, 프로퍼티>
	};

	bool IsSkip(std::ifstream& file, std::string& name);
	void Skip(std::ifstream& file);

	DLLEXPORT std::shared_ptr<Property> Get(const std::string& path);
	DLLEXPORT std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, const std::string& path);
	DLLEXPORT int GetInt(const std::shared_ptr<Property>& prop, const std::string& path = "");
	DLLEXPORT INT2 GetInt2(const std::shared_ptr<Property>& prop, const std::string& path = "");
	DLLEXPORT float GetFloat(const std::shared_ptr<Property>& prop, const std::string& path = "");
	DLLEXPORT std::string GetString(const std::shared_ptr<Property>& prop, const std::string& path = "");
}