#pragma once

class Clipboard : public TSingleton<Clipboard>
{
public:
	Clipboard() = default;
	~Clipboard() = default;

	void Copy(const std::vector<std::shared_ptr<Resource::Property>>& sources);
	void Paste(const std::shared_ptr<Resource::Property>& destination) const;

private:
	std::vector<std::shared_ptr<Resource::Property>> m_sources;
};
