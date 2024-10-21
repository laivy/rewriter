#pragma once

class Clipboard : public TSingleton<Clipboard>
{
public:
	Clipboard() = default;
	~Clipboard() = default;

	void Copy(const std::vector<std::shared_ptr<Resource::Property>>& targets);
	void Paste(const std::shared_ptr<Resource::Property>& prop) const;
	void Clear();

private:
	std::vector<std::shared_ptr<Resource::Property>> m_sources;
};
