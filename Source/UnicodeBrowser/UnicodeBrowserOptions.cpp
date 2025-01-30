// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowserOptions.h"

#include "DataAsset_FontTags.h"
#include "PropertyEditorModule.h"

#include "Engine/Font.h"

#include "Modules/ModuleManager.h"

FSlateFontInfo& UUnicodeBrowserOptions::GetFontInfo()
{
	FontInfo.FontObject = Font;
	return FontInfo;
}

void UUnicodeBrowserOptions::SetFontInfo(FSlateFontInfo const& FontInfoIn)
{
	Font = Cast<UFont>(FontInfoIn.FontObject);
	FontInfo = FontInfoIn;

	if (Font)
	{
		bool bHasValidTypeface = false;
		for (auto& Typeface : Font->CompositeFont.DefaultTypeface.Fonts)
		{
			if (Typeface.Name == FontTypeFace)
			{
				bHasValidTypeface = true;
				break;
			}
		}

		// set to default if the current isn't valid
		if (!bHasValidTypeface)
		{
			FontTypeFace = Font->CompositeFont.DefaultTypeface.Fonts.Num() > 0 ? Font->CompositeFont.DefaultTypeface.Fonts[0].Name : "Default";
		}
	}
	else
	{
		FontTypeFace = NAME_None;
	}

	OnFontChanged.Broadcast();
}

void UUnicodeBrowserOptions::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, Preset))
	{
		// set the first font from the preset if it's bound to a specific font
		if (Preset && Preset->Fonts.Num() > 0 && Font != Preset->Fonts[0])
		{
			FontInfo.FontObject = Preset->Fonts[0];
			SetFontInfo(FontInfo); // this triggers all necessary updates
		}
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, Font))
	{
		FontInfo.FontObject = Font;
		if (!FontInfo.HasValidFont())
		{
			FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		}
		SetFontInfo(FontInfo); // this triggers all necessary updates
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, FontTypeFace))
	{
		if (!FontInfo.HasValidFont())
		{
			FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		}

		FontInfo.TypefaceFontName = FontTypeFace;
		SetFontInfo(FontInfo); // this triggers all necessary updates
	}
}

UUnicodeBrowserOptions* UUnicodeBrowserOptions::Get()
{
	return GetMutableDefault<UUnicodeBrowserOptions>();
}

TArray<FString> UUnicodeBrowserOptions::GetTypeFaces() const
{
	TArray<FString> Result;
	if (Font)
	{
		for (auto& Entry : Font->CompositeFont.DefaultTypeface.Fonts)
		{
			Result.Add(Entry.Name.ToString());
		}
	}

	return Result;
}

void UUnicodeBrowserOptions::PostInitProperties()
{
	Super::PostInitProperties();
	if (!FontInfo.HasValidFont())
	{
		FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}

	if (!Font)
	{
		Font = Cast<UFont>(FontInfo.FontObject);
	}
}
