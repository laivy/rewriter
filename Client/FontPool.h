#pragma once

class FontPool : public TSingleton<FontPool>
{
public:
	enum Type
	{
		DEFAULT, MORRIS
	};

public:
	FontPool();
	~FontPool();

	ComPtr<IDWriteTextFormat> GetFont(Type type) const;

private:
	struct FontInfo
	{
		std::wstring fontFamilyName;
		ComPtr<IDWriteFontCollection1> fontCollection;
	};

private:
	FontInfo LoadFont(const std::wstring& filePath);

private:
	std::set<std::wstring> m_filePaths;
	std::unordered_map<INT, ComPtr<IDWriteTextFormat>> m_pool;
};