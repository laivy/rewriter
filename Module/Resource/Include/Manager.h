#pragma once

namespace Resource
{
	class Property;

	class Manager : public TSingleton<Manager>
	{
	public:
		Manager() = default;
		~Manager() = default;

		std::shared_ptr<Property> Get(const std::wstring& path);
		void Unload(const std::wstring& path = L"");

	private:
		std::unordered_map<std::wstring, std::shared_ptr<Property>> m_resources; // <파일이름, 루트프로퍼티>
	};

	__declspec(dllexport) std::shared_ptr<Property> Get(const std::wstring& path);
	__declspec(dllexport) std::shared_ptr<Property> Load(const std::filesystem::path& path, const std::wstring& subPath = L"");
	__declspec(dllexport) void Unload(const std::wstring& path = L"");
}