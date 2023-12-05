#pragma once
#include "Common/Singleton.h"
#include "Common/Types.h"

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
		bool Load(const std::string& fileName);

	private:
		std::unordered_map<std::string, std::shared_ptr<Property>> m_resources; // <파일이름, 프로퍼티>
	};

	bool IsSkip(std::ifstream& file, std::string& name);
	void Skip(std::ifstream& file);

	__declspec(dllexport) std::shared_ptr<Property> Get(const std::string& path);
	__declspec(dllexport) std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, const std::string& path);
	__declspec(dllexport) void Unload(const std::string& path = "");
}