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
UCLASS(Hidden, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "Font Options")
class UNICODEBROWSER_API UUnicodeBrowserOptions : public UObject, public FNotifyHook
{
	GENERATED_BODY()

public:
	static TSharedRef<class IDetailsView> MakePropertyEditor(UUnicodeBrowserOptions* Options);

	static TSharedRef<SWidget> MakePropertyEditorFont(UUnicodeBrowserOptions* Options);

	UPROPERTY(EditAnywhere, DisplayName="Font")
	FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);

	UPROPERTY(EditAnywhere, Transient, DisplayName="Preset")
	TObjectPtr<UDataAsset_FontTags> Preset;

	UPROPERTY()
	int32 NumCols = 16;

	// Show Characters which can't be displayed by the font
	UPROPERTY()
	bool bShowMissing = false;

	// Show Characters which have a measurement of 0x0 (primary for debug purposes)
	UPROPERTY()
	bool bShowZeroSize = false;

	// Cache the Character meta information while loading the font, this is slower while changing fonts, but may reduce delay for displaying character previews
	UPROPERTY()
	bool bCacheCharacterMetaOnLoad = false;
	

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUbOptionsChangedDelegate, struct FPropertyChangedEvent*);
	FOnUbOptionsChangedDelegate OnChanged;

	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// FNotifyHook Implementation
	virtual void NotifyPostChange( const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override
	{
		OnChanged.Broadcast(nullptr);
	}
	// FNotifyHook Implementation End
};