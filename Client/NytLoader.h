#pragma once

class NytProperty;
class NytImage;

class NytLoader : public TSingleton<NytLoader>
{
public:
	NytLoader() = default;
	~NytLoader() = default;

	NytProperty& Load(const std::string& filePath);
	void Unload(const std::string& filePath);

	void ClearUploadBuffers();

private:
	void Load(std::ifstream& fs, NytProperty& root);

	template<class T>
	T Read(std::ifstream& fs)
	{
		T value{};
		fs.read(reinterpret_cast<char*>(&value), sizeof(T));
		return value;
	}

	template<>
	std::string Read(std::ifstream& fs);

	NytImage Read(std::ifstream& fs, NytDataType type);

private:
	std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
	std::unordered_map<std::string, NytProperty> m_data;
};