// all rights reserved

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "DataAsset_FontTags.generated.h"

/**
 * 
 */

struct FSlateFontInfo;
class UFont;

USTRUCT(BlueprintType)
struct FUnicodeCharacterTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Character = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> Tags;

	bool ContainsNeedle(FString Needle) const
	{
		return Tags.ContainsByPredicate([Needle](FString const& Tag) { return Tag.Contains(Needle, ESearchCase::IgnoreCase); });
	}
};

UCLASS(BlueprintType, Blueprintable)
class UNICODEBROWSER_API UDataAsset_FontTags : public UDataAsset
{
	GENERATED_BODY()

public:
	// a parent asset, tags will be merged into this preset
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDataAsset_FontTags> Parent;

	// the fonts which should be associated with the Tags
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UFont>> Fonts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FUnicodeCharacterTags> Characters;

	// this data is generated at runtime
	mutable TMap<int32, int32> CodepointLookup; // Codepoint <> Characters Index
	mutable TArray<FUnicodeCharacterTags> CharactersMerged;

	// the json file which was used to import the asset
	UPROPERTY(VisibleAnywhere)
	FString SourceFile;

	TArray<FUnicodeCharacterTags>& GetCharactersMerged() const;

	TArray<int32> GetCharactersByNeedle(FString NeedleIn) const;

	bool SupportsFont(FSlateFontInfo const& FontInfo) const;

	void CacheCodepoints() const;

	TArray<FString> GetCodepointTags(int32 Codepoint) const;

	bool ImportFromJson(FString Filename);
};
