// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SUbCheckBoxList.h"
#include "Fonts/UnicodeBlockRange.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeBlockRangeSelector : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBlockRangeSelector) {}
	SLATE_ARGUMENT(TWeakPtr<class SUnicodeBrowserWidget>, UnicodeBrowser)
	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FRangeClicked, EUnicodeBlockRange)
	FRangeClicked OnRangeClicked;

	DECLARE_DELEGATE_TwoParams(FRangeStateChanged, EUnicodeBlockRange, bool)
	FRangeStateChanged OnRangeStateChanged;

	DECLARE_DELEGATE(FRangeSelectionChanged)
	FRangeSelectionChanged OnRangeSelectionChanged;
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/*@param bExclusive Should all other ranges be disabled? */	
	void SetRanges(TArray<EUnicodeBlockRange> RangesToSet, bool bExclusive = true);
	
	bool IsRangeChecked(EUnicodeBlockRange Range) const
	{
		return CheckboxIndices.Contains(Range) ? CheckBoxList->IsItemChecked(CheckboxIndices.FindChecked(Range)) : false;
	}

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
protected:
	bool bSelectionChanged = false;
	TMap<EUnicodeBlockRange const, int32 const> CheckboxIndices; // range <> SUbCheckBoxList index

	TWeakPtr<class SUnicodeBrowserWidget> UnicodeBrowser;
	TSharedPtr<SUbCheckBoxList> CheckBoxList;
	
	void UpdateRangeVisibility(int32 const Index);

	void UpdateRowVisibility(FSlateFontInfo* FontInfo);
	
	FReply RangeClicked(EUnicodeBlockRange const BlockRange) const;
};
