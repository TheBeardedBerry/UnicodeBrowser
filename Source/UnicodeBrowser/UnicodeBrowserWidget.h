// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SUniformGridPanel;
THIRD_PARTY_INCLUDES_START
// Include ICU headers
#include <unicode/uchar.h>
#include <unicode/unistr.h>
THIRD_PARTY_INCLUDES_END
#include "UnicodeBrowserWidget.generated.h"
class UFont;
class SScrollBox;
class SUbCheckBoxList;
class SCheckBoxList;

namespace UnicodeBrowser
{
	inline TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode)
	{
		for (auto const& BlockRange : FUnicodeBlockRange::GetUnicodeBlockRanges())
		{
			if (BlockRange.Range.Contains(CharCode))
			{
				return BlockRange.Index;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("No Unicode block range found for character code %d: %s"), CharCode, *FString::Chr(CharCode));
		return {};
	}
	
	inline TArrayView<FUnicodeBlockRange const> GetUnicodeBlockRanges()
	{
		auto BlockRange = FUnicodeBlockRange::GetUnicodeBlockRanges();
		BlockRange.StableSort([](FUnicodeBlockRange const& A, FUnicodeBlockRange const& B)
		{
			return A.DisplayName.CompareTo(B.DisplayName) < 0;
		});
		return BlockRange;
	}

	inline int32 GetRangeIndex(EUnicodeBlockRange BlockRange)
	{
		return GetUnicodeBlockRanges().IndexOfByPredicate(
			[BlockRange](FUnicodeBlockRange const& Range)
			{
				return Range.Index == BlockRange;
			}
		);
	}

	static TArray<EUnicodeBlockRange> GetSymbolRanges()
	{
		TArray<EUnicodeBlockRange> SymbolRanges;
		SymbolRanges.Add(EUnicodeBlockRange::Arrows);
		SymbolRanges.Add(EUnicodeBlockRange::BlockElements);
		SymbolRanges.Add(EUnicodeBlockRange::BoxDrawing);
		SymbolRanges.Add(EUnicodeBlockRange::CurrencySymbols);
		SymbolRanges.Add(EUnicodeBlockRange::Dingbats);
		SymbolRanges.Add(EUnicodeBlockRange::EmoticonsEmoji);
		SymbolRanges.Add(EUnicodeBlockRange::EnclosedAlphanumericSupplement);
		SymbolRanges.Add(EUnicodeBlockRange::EnclosedAlphanumerics);
		SymbolRanges.Add(EUnicodeBlockRange::GeneralPunctuation);
		SymbolRanges.Add(EUnicodeBlockRange::GeometricShapes);
		SymbolRanges.Add(EUnicodeBlockRange::MathematicalAlphanumericSymbols);
		SymbolRanges.Add(EUnicodeBlockRange::MathematicalOperators);
		SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousMathematicalSymbolsB);
		SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbols);
		SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbolsAndArrows);
		SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbolsAndPictographs);
		SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousTechnical);
		SymbolRanges.Add(EUnicodeBlockRange::NumberForms);
		SymbolRanges.Add(EUnicodeBlockRange::SupplementalSymbolsAndPictographs);
		SymbolRanges.Add(EUnicodeBlockRange::TransportAndMapSymbols);
		SymbolRanges.Add(EUnicodeBlockRange::CurrencySymbols);
		SymbolRanges.Add(EUnicodeBlockRange::Latin1Supplement);
		SymbolRanges.Add(EUnicodeBlockRange::LatinExtendedB);
		SymbolRanges.Sort();
		return SymbolRanges;
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

UCLASS(Hidden, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "Font Options")
class UNICODEBROWSER_API UUnicodeBrowserOptions : public UObject
{
	GENERATED_BODY()
public:

	static TSharedRef<class IDetailsView> MakePropertyEditor(UUnicodeBrowserOptions* Options);
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties), Transient)
	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);

	UPROPERTY(EditAnywhere)
	int32 NumCols = 32;

	virtual void PostInitProperties() override
	{
		Super::PostInitProperties();
		if (!Font.HasValidFont())
		{
			Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		}
	}
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UUnicodeBrowserOptions, NumCols))
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnNumColsChanged.Broadcast();
		}

		if (!Font.HasValidFont())
		{
			Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		}
	}

	DECLARE_MULTICAST_DELEGATE(FOnNumColsChangedDelegate);
	FOnNumColsChangedDelegate OnNumColsChanged;
	
};
/**
 * 
 */
class SUnicodeBrowserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	FReply OnOnlySymbolsClicked();
	void Construct(FArguments const& InArgs);

	TArray<TSharedPtr<FUnicodeBrowserRow>> Rows;
	TMap<uint32, FString> SupportedCharacters;
	TMap<EUnicodeBlockRange const, int32 const> RangeIndices;
	TArrayView<FUnicodeBlockRange const> Ranges;
	TSharedPtr<class IDetailsView> FontDetailsView;
	mutable TMap<int32, TSharedRef<STextBlock>> RowWidgetTextCache;

protected:
	TSharedPtr<SWidget> MakeRangeWidget(FUnicodeBlockRange Range) const;
	TSharedPtr<SUbCheckBoxList> MakeRangeSelector();
	FReply OnRangeClicked(EUnicodeBlockRange BlockRange) const;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TArray<TSharedPtr<SWidget>> RangeWidgets;
	TSharedPtr<SUbCheckBoxList> RangeSelector;
	void PopulateSupportedCharacters();
	FString GetUnicodeCharacterName(int32 CharCode);
	TObjectPtr<UUnicodeBrowserOptions> Options;
	FReply OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const;
	FSlateFontInfo GetFont() const;
	void RebuildGridColumns(::FUnicodeBlockRange Range, TSharedRef<SUniformGridPanel> const GridPanel) const; 
};
