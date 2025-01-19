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
	
	TArray<FUnicodeCharacterTags>& GetCharactersMerged() const
	{
		if(CharactersMerged.IsEmpty())
		{
			CharactersMerged = Characters;
			// create the codepoint cache for quicker lookup of existing entries
			CacheCodepoints();
			
			if(Parent && Parent != this) // there's a chance of infinite recursion here, this still doesn't catch recursion traps like A => B => A
			{			
				for(FUnicodeCharacterTags &ParentCharacter : Parent->GetCharactersMerged())
				{
					// character exists, append tags
					if(int32 *CharacterIndex = CodepointLookup.Find(ParentCharacter.Character))
					{
						for(FString &ParentCharacterTag : ParentCharacter.Tags)
						{
							CharactersMerged[*CharacterIndex].Tags.AddUnique(ParentCharacterTag);
						}
					}
					// character doesn't exist, add it
					else
					{
						CharactersMerged.Add(ParentCharacter);
					}
				}

				// recreate the codepoint cache if the parent has any characters
				if(Parent->GetCharactersMerged().Num() > 0){
					CacheCodepoints();
				}
			}
		}
		return CharactersMerged;
	}
	
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
		
		Result.Reserve(GetCharactersMerged().Num());
		for(FUnicodeCharacterTags const &Entry : GetCharactersMerged())
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
		// allow presets to apply to all fonts if they don't contain any specific fonts
		return Fonts.IsEmpty() || Fonts.Contains(Cast<UFont>(FontInfo.FontObject));
	}

	void CacheCodepoints() const
	{
		CodepointLookup.Reset();
		CodepointLookup.Reserve(CharactersMerged.Num());

		for(int Idx=0; Idx < CharactersMerged.Num(); Idx++)
		{
			CodepointLookup.Add(CharactersMerged[Idx].Character, Idx);
		}
	}

	TArray<FString> GetCodepointTags(int32 Codepoint) const
	{
		if(CodepointLookup.IsEmpty())
		{
			// this creates the cache
			GetCharactersMerged();
		}

		if(int32 *Index = CodepointLookup.Find(Codepoint))
		{
			return GetCharactersMerged()[*Index].Tags; 			
		}

		return {};
	}
	
};
