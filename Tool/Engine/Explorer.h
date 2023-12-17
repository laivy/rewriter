#pragma once

class Explorer : public TSingleton<Explorer>
{
public:
	Explorer();
	~Explorer() = default;

	void Render();

private:
	std::filesystem::path m_path;
};