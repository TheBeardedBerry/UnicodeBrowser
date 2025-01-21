// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "DataAsset_FontTags.h"

#include "Styling/CoreStyle.h"

#include "UObject/Object.h"

#include "UnicodeBrowserOptions.generated.h"

/**
 * 
 */
UCLASS(Config=Engine, defaultconfig, meta = (DisplayName="UnicodeBrowser"))
class UNICODEBROWSER_API UUnicodeBrowserOptions : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(DisplayName="Preset")
	TObjectPtr<UDataAsset_FontTags> Preset;

	UPROPERTY(Config, EditAnywhere, meta=(UIMin=1))
	int32 NumCols = 16;

	// Show Characters which can't be displayed by the font
	UPROPERTY(Config, EditAnywhere)
	bool bShowMissing = false;

	// Show Characters which have a measurement of 0x0 (primary for debug purposes)
	UPROPERTY(Config, EditAnywhere)
	bool bShowZeroSize = false;

	// Cache the Character meta information while loading the font, this is slower while changing fonts, but may reduce delay for displaying character previews
	UPROPERTY(Config, EditAnywhere)
	bool bCacheCharacterMetaOnLoad = false;

	// pick unicode range based on what's available in the font
	UPROPERTY(Config, EditAnywhere)
	bool bAutoSetRangeOnFontChange = false;
	
	FSlateFontInfo& GetFontInfo()
	{
		FontInfo.FontObject = Font;
		return FontInfo;
	}

	void SetFontInfo(FSlateFontInfo &FontInfoIn)
	{
		Font = Cast<UFont>(FontInfoIn.FontObject);
		FontInfo = FontInfoIn;

		if(Font)
		{
			bool bHasValidTypeface = false;
			for(auto &Typeface : Font->CompositeFont.DefaultTypeface.Fonts)
			{
				if(Typeface.Name == FontTypeFace)
				{
					bHasValidTypeface = true;
					break;
				}	
			}

			// set to default if the current isn't valid
			if(!bHasValidTypeface){
				FontTypeFace = Font->CompositeFont.DefaultTypeface.Fonts[0].Name;
			}
		}
		else
		{
			FontTypeFace = NAME_None;	
		}
		
		
		OnFontChanged.Broadcast();
	}
		
	DECLARE_MULTICAST_DELEGATE(FOnUbOptionsChangedDelegate);
	FOnUbOptionsChangedDelegate OnFontChanged;

	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, Preset))
		{
			// set the first font from the preset if it's bound to a specific font
			if(Preset && Preset->Fonts.Num() > 0 && Font != Preset->Fonts[0])
			{
				FontInfo.FontObject = Preset->Fonts[0];
				SetFontInfo(FontInfo); // this triggers all necessary updates
			}
		}
		
		if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, Font))
		{
			FontInfo.FontObject = Font;
			if (!FontInfo.HasValidFont())
			{
				FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
			}
			SetFontInfo(FontInfo); // this triggers all necessary updates
		}

		if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, FontTypeFace))
		{
			if (!FontInfo.HasValidFont())
			{
				FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
			}

			FontInfo.TypefaceFontName = FontTypeFace;			
			SetFontInfo(FontInfo); // this triggers all necessary updates
		}
		
	}



private:
	UPROPERTY(Transient, DisplayName="Font")
	FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);

	UPROPERTY(Transient, DisplayName="Font")
	TObjectPtr<UFont const> Font = nullptr;

	UPROPERTY(Transient, DisplayName="FontTypeface", meta=(EditCondition="Font", GetOptions=GetTypeFaces))
	FName FontTypeFace = NAME_None;

	UFUNCTION()
	TArray<FString> GetTypeFaces() const
	{
		TArray<FString> Result;
		if(Font){
			for(auto &Entry : Font->CompositeFont.DefaultTypeface.Fonts)
			{
				Result.Add(Entry.Name.ToString());
			}
		}

		return Result;
	}
};