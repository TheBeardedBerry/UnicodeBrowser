// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/Font.h"
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
	int32 Character;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	TArray<FString> Tags;

	bool ContainsNeedle(FString Needle) const
	{
		return Tags.ContainsByPredicate([Needle](FString const &Tag){ return Tag.Contains(Needle, ESearchCase::IgnoreCase); });
	}
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
	TArray<FUnicodeCharacterTags> Characters;

	mutable TMap<int32, int32> CodepointLookup; // Codepoint <> Characters Index

	TArray<int32> GetCharactersByNeedle(FString NeedleIn) const
	{
		TArray<int32> Result;
		TArray<FString> Needles;

		// explode only if the length is >1 since "," is a valid character search term
		if(NeedleIn.Len() > 1)
		{
			NeedleIn.ParseIntoArray(Needles, TEXT(","));
		}
		else
		{
			Needles = { NeedleIn };
		}

		// trim, as the user may type stuff like "Phone, Calculator"
		for(FString &Needle : Needles)
		{
			 Needle.TrimStartAndEndInline();
		}
		
		Result.Reserve(Characters.Num());
		for(FUnicodeCharacterTags const &Entry : Characters)
		{
			for(FString const &Needle : Needles)
			{
				if(Entry.ContainsNeedle(Needle))
				{
					Result.Add(Entry.Character);
					break;
				}
			}
		}

		return Result;
	}

	bool SupportsFont(FSlateFontInfo const &FontInfo) const
	{
		return Fonts.Contains(Cast<UFont>(FontInfo.FontObject));
	}

	void CacheCodepoints() const
	{
		CodepointLookup.Reset();
		CodepointLookup.Reserve(Characters.Num());

		for(int Idx=0; Idx < Characters.Num(); Idx++)
		{
			CodepointLookup.Add(Characters[Idx].Character, Idx);
		}
	}

	TArray<FString> GetCodepointTags(int32 Codepoint) const
	{
		if(CodepointLookup.IsEmpty() && Characters.Num() > 0)
		{
			CacheCodepoints();
		}

		if(int32 *Index = CodepointLookup.Find(Codepoint))
		{
			return Characters[*Index].Tags; 			
		}

		return {};
	}
	
};
