// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowserOptions.h"
#include "UnicodeBrowserRow.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SExpandableArea;
class UFont;
class SScrollBox;
class SUbCheckBoxList;
class SCheckBoxList;
class SUniformGridPanel;

namespace UnicodeBrowser
{
	TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode);

	TArrayView<FUnicodeBlockRange const> GetUnicodeBlockRanges();

	int32 GetRangeIndex(EUnicodeBlockRange BlockRange);

	static TArray<EUnicodeBlockRange> SymbolRanges = {
		EUnicodeBlockRange::Arrows,
		EUnicodeBlockRange::BlockElements,
		EUnicodeBlockRange::BoxDrawing,
		EUnicodeBlockRange::CurrencySymbols,
		EUnicodeBlockRange::Dingbats,
		EUnicodeBlockRange::EmoticonsEmoji,
		EUnicodeBlockRange::EnclosedAlphanumericSupplement,
		EUnicodeBlockRange::EnclosedAlphanumerics,
		EUnicodeBlockRange::GeneralPunctuation,
		EUnicodeBlockRange::GeometricShapes,
		EUnicodeBlockRange::Latin1Supplement,
		EUnicodeBlockRange::LatinExtendedB,
		EUnicodeBlockRange::MathematicalAlphanumericSymbols,
		EUnicodeBlockRange::MathematicalOperators,
		EUnicodeBlockRange::MiscellaneousMathematicalSymbolsB,
		EUnicodeBlockRange::MiscellaneousSymbols,
		EUnicodeBlockRange::MiscellaneousSymbolsAndArrows,
		EUnicodeBlockRange::MiscellaneousSymbolsAndPictographs,
		EUnicodeBlockRange::MiscellaneousTechnical,
		EUnicodeBlockRange::NumberForms,
		EUnicodeBlockRange::SupplementalSymbolsAndPictographs,
		EUnicodeBlockRange::TransportAndMapSymbols
	};
}

class SUnicodeBrowserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);
	
protected:
	TArrayView<FUnicodeBlockRange const> Ranges; // all known Unicode ranges
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> Rows;	
	TMap<EUnicodeBlockRange, TSharedPtr<SExpandableArea>> RangeWidgets;
	TMap<EUnicodeBlockRange, TSharedPtr<SUniformGridPanel>> RangeWidgetsGrid;
	TMap<EUnicodeBlockRange const, int32 const> RangeIndices; // range <> SUbCheckBoxList index 
	TObjectPtr<UUnicodeBrowserOptions> Options;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TSharedPtr<SUbCheckBoxList> RangeSelector;
	TSharedPtr<class IDetailsView> FontDetailsView;
	mutable TMap<int32, TSharedRef<SBorder>> RowWidgetTextCache;
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;
	TSharedPtr<STextBlock> CurrentCharacterView;

protected:
	FReply OnCurrentCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent) const;
	FReply OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const;
	FReply OnOnlySymbolsClicked();
	FReply OnRangeClicked(EUnicodeBlockRange BlockRange) const;
	FReply OnCharacterMouseMove(::FGeometry const& Geometry, ::FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;
	FSlateFontInfo GetFont() const;
	FString GetUnicodeCharacterName(int32 CharCode);
	TSharedPtr<SUbCheckBoxList> MakeRangeSelector();
	void MakeRangeWidget(FUnicodeBlockRange Range);
	void PopulateSupportedCharacters();
	void RebuildGridColumns(::FUnicodeBlockRange Range, TSharedRef<SUniformGridPanel> const GridPanel) const;

	
	void UpdateFromFont(struct FPropertyChangedEvent* PropertyChangedEvent = nullptr);
};

