// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DataAsset_FontTags.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FUnicodeCharacterTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	TArray<FString> Tags;
};

UCLASS(BlueprintType, Blueprintable)
class UNICODEBROWSER_API UDataAsset_FontTags : public UDataAsset
{
	GENERATED_BODY()

public:
	// the fonts which should be associated with the Tags
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UFont>> Fonts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, FUnicodeCharacterTags> Characters;
};
