#pragma once

class NytProperty;
class NytImage;

class NytLoader : public TSingleton<NytLoader>
{
public:
	NytLoader();

	NytProperty& Load(const std::string& filePath);

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

	template<>
	NytImage Read(std::ifstream& fs);

private:
	ComPtr<IWICImagingFactory> m_WICIFactory;
	std::unordered_map<std::string, NytProperty> m_data;
};