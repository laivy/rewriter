#pragma once

namespace Resource
{
	class Image;
	class Property;

	class ResourceManager : public TSingleton<ResourceManager>
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		std::shared_ptr<Property> Get(const std::string& fileName);
		void Unload(const std::string& path = "");

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
	DLLEXPORT Image* GetImage(const std::shared_ptr<Property>& prop, const std::string& path = "");
	DLLEXPORT void Unload(const std::string& path = "");
}