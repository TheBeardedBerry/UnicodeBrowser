// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowserOptions.h"
#include "UnicodeBrowserRow.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/SUbSearchBar.h"
#include "Widgets/SUnicodeRangeWidget.h"
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

	static TArrayView<FUnicodeBlockRange const> Ranges; // all known Unicode ranges	
	TArrayView<FUnicodeBlockRange const> GetUnicodeBlockRanges();
	
	static FString GetUnicodeCharacterName(int32 CharCode);

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
	friend class SUnicodeBrowserSidePanel;
	
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FHighlightCharacter, FUnicodeBrowserRow*)
	FHighlightCharacter OnCharacterHighlight;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FFontChanged, FSlateFontInfo*)
	FFontChanged OnFontChanged;

	FSlateFontInfo DefaultFont = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> RowsRaw;

	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> Rows;

public:
	void Construct(FArguments const& InArgs);
	virtual void Tick(FGeometry const& AllottedGeometry, double const InCurrentTime, float const InDeltaTime) override;
	void MarkDirty();
	
	TObjectPtr<UUnicodeBrowserOptions> Options;
	
protected:	
	TMap<EUnicodeBlockRange, TSharedPtr<SUnicodeRangeWidget>> RangeWidgets;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TSharedPtr<SUbSearchBar> SearchBar;
	TSharedPtr<class SUnicodeBrowserSidePanel> SidePanel;
	
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;
	FSlateFontInfo CurrentFont = DefaultFont;
	
	bool bDirty = true;
	
protected:
	void Update();
	void PopulateSupportedCharacters();
	void UpdateCharacters();
	void RebuildGridRange(TSharedPtr<SUnicodeRangeWidget> RangeWidget);	
	void RebuildGrid();

	void FilterByString(FString Needle);
	
	FReply OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;
	void HandleZoomFont(float Offset);
	void HandleZoomColumns(float Offset);	
};

