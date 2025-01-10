// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SCheckBoxList;
THIRD_PARTY_INCLUDES_START
// Include ICU headers
#include <unicode/unistr.h>
#include <unicode/uchar.h>
THIRD_PARTY_INCLUDES_END
#include "UnicodeBrowserWidget.h"

namespace UnicodeBrowser
{
	inline TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode)
	{
		for (auto const& BlockRange :  FUnicodeBlockRange::GetUnicodeBlockRanges())
		{
			if (BlockRange.Range.Contains(CharCode))
			{
				return BlockRange.Index;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("No Unicode block range found for character code %d: %s"), CharCode, *FString::Chr(CharCode));
		return {};
	}

	inline int32 GetRangeIndex(EUnicodeBlockRange BlockRange)
	{
		return FUnicodeBlockRange::GetUnicodeBlockRanges().IndexOfByPredicate(
			[BlockRange](const FUnicodeBlockRange& Range)
			{
				return Range.Index == BlockRange;
			}
		);
	}
}
class FUnicodeBrowserRow : public TSharedFromThis<FUnicodeBrowserRow>
{
public:
	FUnicodeBrowserRow(int32 const InCharCode, FString const& InCharacter, EUnicodeBlockRange InBlockRange)
		: CharCode(InCharCode),
		Character(InCharacter)
	{
		BlockRange = UnicodeBrowser::GetUnicodeBlockRangeFromChar(InCharCode);
	}

	int32 CharCode;
	FString Character;
	TOptional<EUnicodeBlockRange> BlockRange;
};

/**
 * 
 */
class SUnicodeBrowserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);

	TArray<TSharedPtr<FUnicodeBrowserRow>> Rows;
	TMap<uint32, FString> SupportedCharacters;

private:
	TSharedPtr<SWidget> MakeRangeWidget(EUnicodeBlockRange BlockRange, FPlatformTypes::uint32 NumCols, TSharedPtr<SCheckBoxList> RangeSelector) const;
	TSharedPtr<SCheckBoxList> MakeRangeSelector() const;
	void PopulateSupportedCharacters();
	FString GetUnicodeCharacterName(int32 CharCode);
};
