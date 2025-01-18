// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"

class FUnicodeBrowserRow;
class IDetailsView;
class SCheckBoxList;
class SExpandableArea;
class SScrollBox;
class STextBlock;
class SUbCheckBoxList;
class SUnicodeRangeWidget;
class SUniformGridPanel;
class UFont;
class UUnicodeBrowserOptions;
enum class EUnicodeBlockRange : uint16;
struct FUnicodeBlockRange;

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
	TMap<EUnicodeBlockRange, TSharedPtr<SUnicodeRangeWidget>> RangeWidgets;
	TObjectPtr<UUnicodeBrowserOptions> Options;
	TSharedPtr<IDetailsView> FontDetailsView;
	TSharedPtr<SExpandableArea> BlockRangesSidebar;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TSharedPtr<STextBlock> CurrentCharacterView;
	TSharedPtr<SUbCheckBoxList> RangeSelector;
	bool bDirty = true;
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;

protected:
	FReply OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const;
	FReply OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;
	FReply OnCurrentCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent) const;
	FReply OnOnlyBlocksWithCharactersClicked() const;
	FReply OnOnlySymbolsClicked();
	FReply OnRangeClicked(EUnicodeBlockRange BlockRange) const;
	void HandleFontChanged();
	void HandleZoomColumns(float Offset);
	void HandleZoomFont(float Offset);
	void MarkDirty();
	void UpdateRangeVisibility(int32 Index);
	FSlateFontInfo GetFontInfo() const;

	TSharedPtr<SExpandableArea> MakeBlockRangesSidebar();
	TSharedPtr<SUbCheckBoxList> MakeBlockRangeSelector();

	void PopulateSupportedCharacters();
	void RebuildGrid();
	void RebuildGridRange(TSharedPtr<SUnicodeRangeWidget> RangeWidget) const;
	void SelectAllRangesWithCharacters() const;
	void UpdateFromFont(FPropertyChangedEvent* PropertyChangedEvent = nullptr);
};
