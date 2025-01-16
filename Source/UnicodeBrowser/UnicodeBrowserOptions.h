// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Styling/CoreStyle.h"

#include "UObject/Object.h"

#include "UnicodeBrowserOptions.generated.h"

/**
 * 
 */
UCLASS(Hidden, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "Font Options")
class UNICODEBROWSER_API UUnicodeBrowserOptions : public UObject
{
	GENERATED_BODY()

public:
	static TSharedRef<class IDetailsView> MakePropertyEditor(UUnicodeBrowserOptions* Options);
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties), Transient)
	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);

	UPROPERTY(EditAnywhere)
	int32 NumCols = 24;

	// Show Characters which can't be displayed by the font (primary for debug purposes)
	UPROPERTY(EditAnywhere)
	bool bShowMissing = false;

	// Show Characters which have a measurement of 0x0 (primary for debug purposes)
	UPROPERTY(EditAnywhere)
	bool bShowZeroSize = false;

	// Cache the Character meta information while loading the font, this is slower while changing fonts, but may reduce delay for displaying character previews
	UPROPERTY(EditAnywhere)
	bool bCacheCharacterMetaOnLoad = false;

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNumColsChangedDelegate, struct FPropertyChangedEvent*);
	FOnNumColsChangedDelegate OnChanged;
};