#pragma once

enum class FontType
{
	DEFAULT,
	MORRIS12
};

class FontPool : public TSingleton<FontPool>
{
public:
	FontPool();
	~FontPool();

	ComPtr<IDWriteTextFormat> GetFont(FontType type);

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
	std::unordered_map<int, ComPtr<IDWriteTextFormat>> m_pool;
};