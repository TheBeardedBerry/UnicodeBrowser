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
	TCHAR constexpr InvalidSubChar = TEXT('\uFFFD');
	TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode);
	
	static FString GetUnicodeCharacterName(int32 CharCode);

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
	virtual void Tick(FGeometry const& AllottedGeometry, double const InCurrentTime, float const InDeltaTime) override;
	void Update();

protected:
	FDelegateHandle OnOptionsChangedHandle;
	TArrayView<FUnicodeBlockRange const> Ranges; // all known Unicode ranges
	TMap<EUnicodeBlockRange const, int32 const> CheckboxIndices; // range <> SUbCheckBoxList index 
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> Rows;
	TMap<EUnicodeBlockRange, TSharedPtr<SExpandableArea>> RangeWidgets;
	TMap<EUnicodeBlockRange, TSharedPtr<SUniformGridPanel>> RangeWidgetsGrid;
	TObjectPtr<UUnicodeBrowserOptions> Options;
	TSharedPtr<IDetailsView> FontDetailsView;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TSharedPtr<STextBlock> CurrentCharacterView;
	TSharedPtr<SUbCheckBoxList> RangeSelector;
	TSharedPtr<SExpandableArea> BlockRangesSidebar;
	bool bDirty = true;
	mutable TMap<int32, TSharedRef<SCompoundWidget>> RowCellWidgetCache;
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;

protected:
	FReply OnCurrentCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent) const;
	FReply OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const;
	FReply OnOnlySymbolsClicked();
	FReply OnOnlyBlocksWithCharactersClicked() const;
	void UpdateRangeVisibility(int32 Index);
	void MarkDirty();
	FReply OnRangeClicked(EUnicodeBlockRange BlockRange) const;
	FReply OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;

	FSlateFontInfo GetFontInfo() const;

	TSharedPtr<SExpandableArea> MakeBlockRangesSidebar();
	TSharedPtr<SUbCheckBoxList> MakeBlockRangeSelector();

	void MakeRangeWidget(FUnicodeBlockRange Range);
	void PopulateSupportedCharacters();
	void RebuildGrid(FUnicodeBlockRange Range, TSharedRef<SUniformGridPanel> const GridPanel) const;
	void UpdateFromFont(FPropertyChangedEvent* PropertyChangedEvent = nullptr);
	void SelectAllRangesWithCharacters() const;
};

