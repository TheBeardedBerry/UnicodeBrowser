// all rights reserved

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditAnywhere)
	bool bShowMissing = false;

	UPROPERTY(EditAnywhere)
	bool bShowZeroSize = false;

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNumColsChangedDelegate, struct FPropertyChangedEvent*);
	FOnNumColsChangedDelegate OnChanged;
};