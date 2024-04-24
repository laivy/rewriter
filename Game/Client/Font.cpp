#include "Stdafx.h"
#include "Font.h"
#include "Renderer.h"

Font::Font(const std::string& fontPath, float size, DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style, DWRITE_FONT_STRETCH stretch)
{
	auto dwriteFactory{ Renderer::dwriteFactory };

	ComPtr<IDWriteFontFile> fontFile;
	dwriteFactory->CreateFontFileReference(TextUtil::str2wstr(fontPath).c_str(), nullptr, &fontFile);

	ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
	dwriteFactory->CreateFontSetBuilder(&fontSetBuilder);
	fontSetBuilder->AddFontFile(fontFile.Get());

	ComPtr<IDWriteFontSet> fontSet;
	fontSetBuilder->CreateFontSet(&fontSet);

	ComPtr<IDWriteFontCollection1> fontCollection;
	dwriteFactory->CreateFontCollectionFromFontSet(fontSet.Get(), &fontCollection);

	std::wstring fontFamilyName;
	fontFamilyName.resize(LF_FACESIZE);
	for (UINT32 i = 0; i < fontCollection->GetFontFamilyCount(); ++i)
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
				dwriteFactory->CreateTextFormat(
					fontFamilyName.c_str(),
					fontCollection.Get(),
					weight,
					style,
					stretch,
					size,
					TEXT(""),
					&m_textFormat
				);
				return;
			}
		}
	}
}

ComPtr<IDWriteTextFormat> Font::GetTextFormat() const
{
	return m_textFormat;
}
