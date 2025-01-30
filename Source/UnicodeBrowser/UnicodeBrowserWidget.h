// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Engine/TimerHandle.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/SUbSearchBar.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STileView.h"

class UToolMenu;
class FUnicodeBrowserRow;
class IDetailsView;
class SCheckBoxList;
class SExpandableArea;
class SScrollBox;
class STextBlock;
class SUbCheckBoxList;
class SUnicodeRangeWidget;
class SUniformGridPanel;

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

	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> RowsRaw; // a raw list of all characters for the current font
	TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> Rows; // a filtered view of the raw data, those are the characters that a

	bool bShouldDisableThrottle = false;
	FDelegateHandle DisableCPUThrottleHandle;
	FTimerHandle ReenableThrottleHandle;

public:
	void Construct(FArguments const& InArgs);
	virtual ~SUnicodeBrowserWidget() override;

	void MarkDirty(uint8 Flags);
	TSharedRef<ITableRow> GenerateItemRow(TSharedPtr<FUnicodeBrowserRow> CharacterData, TSharedRef<STableViewBase> const& OwnerTable);

	virtual void Tick(FGeometry const& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;
	virtual FReply OnMouseMove(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent) override;

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

	FReply OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row);
	void OnCharactersTileViewScrolled(double X);
	void DisableThrottlingTemporarily();

	void HandleZoomFont(float Offset);
	void HandleZoomPadding(float Offset);
	bool ShouldDisableCPUThrottling() const;
	void SetUpDisableCPUThrottlingDelegate();
	void CleanUpDisableCPUThrottlingDelegate();

private:
	UToolMenu* CreateMenuSection_Settings();
	void SetSidePanelVisibility(bool bVisible = true);
	uint8 DirtyFlags;
};
