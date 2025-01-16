// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

#include "UnicodeBrowserWidget.generated.h"

class UFont;
class SScrollBox;
class SUbCheckBoxList;
class SCheckBoxList;
class SUniformGridPanel;

namespace UnicodeBrowser
{
	inline TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode);

	TArrayView<FUnicodeBlockRange const> GetUnicodeBlockRanges();

	inline int32 GetRangeIndex(EUnicodeBlockRange BlockRange)
	{
		return GetUnicodeBlockRanges().IndexOfByPredicate(
			[BlockRange](FUnicodeBlockRange const& Range)
			{
				return Range.Index == BlockRange;
			}
		);
	}

	static TArray<EUnicodeBlockRange> GetSymbolRanges();
}

class FUnicodeBrowserRow : public TSharedFromThis<FUnicodeBrowserRow>
{
public:
	FUnicodeBrowserRow() = default;

	FUnicodeBrowserRow(int32 CodePointIn, TOptional<EUnicodeBlockRange> BlockRangeIn) : Codepoint(CodePointIn), BlockRange(BlockRangeIn) {}

	int32 Codepoint = 0;
	FString Character = "";
	TOptional<EUnicodeBlockRange> BlockRange;
	FFontData FontData;
	float ScalingFactor = 1.0f;
	FVector2D Measurements;
	bool bCanLoadCodepoint = false;

	friend bool operator==(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS)
	{
		return Lhs.Codepoint == RHS.Codepoint
			&& Lhs.Character == RHS.Character
			&& Lhs.BlockRange == RHS.BlockRange
			&& Lhs.FontData == RHS.FontData
			&& Lhs.ScalingFactor == RHS.ScalingFactor
			&& Lhs.Measurements == RHS.Measurements
			&& Lhs.bCanLoadCodepoint == RHS.bCanLoadCodepoint;
	}

	friend bool operator!=(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS) { return !(Lhs == RHS); }
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
	int32 NumCols = 24;

	UPROPERTY(EditAnywhere)
	bool bShowMissing = false;

	UPROPERTY(EditAnywhere)
	bool bShowZeroSize = false;

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	DECLARE_MULTICAST_DELEGATE(FOnNumColsChangedDelegate);
	FOnNumColsChangedDelegate OnChanged;
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
	
protected:
	TArrayView<FUnicodeBlockRange const> Ranges; // all known unicode ranges
	
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

	
	void UpdateFromFont();
};


class SUnicodeCharacterInfo: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeCharacterInfo) {}
		SLATE_ATTRIBUTE(TSharedPtr<FUnicodeBrowserRow>, Row);
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);
	
	TAttribute<TSharedPtr<FUnicodeBrowserRow>> Row;
	TSharedPtr<FUnicodeBrowserRow> GetRow() const;
	
	void SetRow(TSharedPtr<FUnicodeBrowserRow> InRow);
};
