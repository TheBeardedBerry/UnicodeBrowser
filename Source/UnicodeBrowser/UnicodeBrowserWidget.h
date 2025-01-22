// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowserRow.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/SUbSearchBar.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STileView.h"

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

	enum class EDirtyFlags : uint8
	{
		YOLO = 0,
		INIT = 1 << 0,
		FONT_FACE = 1 << 1, // never use this to call MarkDirt, use FONT which also invalidates the style
		FONT_STYLE = 1 << 2,
		FONT = FONT_FACE | FONT_STYLE,
		TILEVIEW_GRID_SIZE = 1 << 3
	};
	
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FHighlightCharacter, FUnicodeBrowserRow*)
	FHighlightCharacter OnCharacterHighlight;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FFontChanged, FSlateFontInfo*)
	FFontChanged OnFontChanged;

	FSlateFontInfo DefaultFont = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> RowsRaw;	// a raw list of all characters for the current font
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> Rows;		// a filtered view of the raw data, those are the characters that a

public:
	virtual ~SUnicodeBrowserWidget() override;
	void Construct(FArguments const& InArgs);
	virtual void Tick(FGeometry const& AllottedGeometry, double const InCurrentTime, float const InDeltaTime) override;
	void MarkDirty(uint8 Flags);
	
	TSharedRef<ITableRow> GenerateItemRow(TSharedPtr<FUnicodeBrowserRow> CharacterData, const TSharedRef<STableViewBase>& OwnerTable);

protected:	
	TArray<TSharedPtr<FUnicodeBrowserRow>> CharacterWidgetsArray;
	TSharedPtr<STileView<TSharedPtr<FUnicodeBrowserRow>>> CharactersTileView;
	TSharedPtr<SUbSearchBar> SearchBar;
	TSharedPtr<class SUnicodeBrowserSidePanel> SidePanel;
	
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;
	FSlateFontInfo CurrentFont = DefaultFont;
	
protected:
	void PopulateSupportedCharacters();
	void UpdateCharacters();
	void UpdateCharactersArray();

	void FilterByString(FString Needle);

	
	FReply OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;
	void HandleZoomFont(float Offset);
	void HandleZoomPadding(float Offset);

private:
	UToolMenu* CreateMenuSection_Settings();
	void SetSidePanelVisibility(bool bVisible = true);
	uint8 DirtyFlags;
};


