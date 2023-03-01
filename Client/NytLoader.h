#pragma once

class NytImage;
class NytProperty;
class NytUI;

class NytLoader : public TSingleton<NytLoader>
{
public:
	NytLoader() = default;
	~NytLoader() = default;

	NytProperty* Load(const std::string& filePath);
	void Unload(const std::string& filePath);

	void CreateShaderResourceView();
	void ClearUploadBuffers();

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12Resource* resource);

private:
	void Load(std::ifstream& fs, NytProperty* root);

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
	NytUI Read(std::ifstream& fs);

	template<>
	NytImage Read(std::ifstream& fs);

private:
	// DX12 리소스 관리를 위한 멤버 변수
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	std::set<ID3D12Resource*> m_srvData;
	std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;

	// 로딩한 데이터
	std::unordered_map<std::string, std::unique_ptr<NytProperty>> m_data;
};