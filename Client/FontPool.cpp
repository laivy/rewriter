#include "Stdafx.h"
#include "FontPool.h"
#include "GameApp.h"

FontPool::FontPool()
{
	// 로딩할 폰트 개수
	constexpr size_t LOAD_FONT_COUNT = 1;

	// 로딩한 폰트로 만들 포멧 정보 구조체
	struct TextFormatInfo
	{
		DWRITE_FONT_WEIGHT fontWeight;
		DWRITE_FONT_STYLE fontStype;
		DWRITE_FONT_STRETCH fontStretch;
		FLOAT fontSize;
		std::wstring localeName;
		IDWriteTextFormat** textFormat;
	};

	// 폰트 파일 경로와 폰트 하나 당 만들 포멧 정보 구조체
	struct FontLoadInfo
	{
		std::wstring fontPath;
		std::vector<TextFormatInfo> textFormatInfoList;
	};

	const std::array<FontLoadInfo, LOAD_FONT_COUNT> fontLoadInfoList
	{
		{ 
			TEXT("Data/morris9.ttf"), 
			{ 
				{ DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, TEXT(""), &m_pool[MORRIS] }
			}
		}
	};

	// 폰트 로딩
	auto dwriteFactory{ GameApp::GetInstance()->GetDwriteFactory() };
	for (const auto& fontLoadInfo : fontLoadInfoList)
	{
		auto fontInfo = LoadFont(fontLoadInfo.fontPath);
		for (const auto& textFormatInfo : fontLoadInfo.textFormatInfoList)
		{
			dwriteFactory->CreateTextFormat(
				fontInfo.fontFamilyName.c_str(),
				fontInfo.fontCollection.Get(),
				textFormatInfo.fontWeight,
				textFormatInfo.fontStype,
				textFormatInfo.fontStretch,
				textFormatInfo.fontSize,
				textFormatInfo.localeName.c_str(),
				textFormatInfo.textFormat
			);
		}
	}
}

FontPool::~FontPool()
{
	for (const auto& filePath : m_filePaths)
		RemoveFontResource(filePath.c_str());
}

ComPtr<IDWriteTextFormat> FontPool::GetFont(Type type) const
{
	if (m_pool.contains(type))
		return m_pool.at(type);
	return m_pool.at(type);
}

FontPool::FontInfo FontPool::LoadFont(const std::wstring& filePath)
{
	auto dwriteFactory{ GameApp::GetInstance()->GetDwriteFactory() };

	ComPtr<IDWriteFontFile> fontFile;
	dwriteFactory->CreateFontFileReference(filePath.c_str(), nullptr, &fontFile);

	ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
	dwriteFactory->CreateFontSetBuilder(&fontSetBuilder);
	fontSetBuilder->AddFontFile(fontFile.Get());

	ComPtr<IDWriteFontSet> fontSet;
	fontSetBuilder->CreateFontSet(&fontSet);

	ComPtr<IDWriteFontCollection1> fontCollection;
	dwriteFactory->CreateFontCollectionFromFontSet(fontSet.Get(), &fontCollection);

	std::wstring fontFamilyName;
	fontFamilyName.resize(LF_FACESIZE);
	for (UINT i = 0; i < fontCollection->GetFontFamilyCount(); ++i)
	{
		ComPtr<IDWriteFontFamily> fontFamily;
		fontCollection->GetFontFamily(i, &fontFamily);

		ComPtr<IDWriteLocalizedStrings> familyName;
		fontFamily->GetFamilyNames(&familyName);
		
		for (UINT j = 0; j < familyName->GetCount(); ++j)
		{
			std::wstring localeName;
			localeName.resize(LOCALE_NAME_MAX_LENGTH);
			familyName->GetLocaleName(j, localeName.data(), static_cast<UINT32>(localeName.length()));

			if (localeName.starts_with(TEXT("en-us")))
			{
				familyName->GetString(j, fontFamilyName.data(), static_cast<UINT32>(fontFamilyName.length()));
				return FontPool::FontInfo{ fontFamilyName, fontCollection };
			}
		}
	}
	return FontPool::FontInfo{};
}
