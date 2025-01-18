// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SUbCheckBoxList.h"
#include "Fonts/UnicodeBlockRange.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeBlockRangeSelector : public SUbCheckBoxList
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBlockRangeSelector) {}
	SLATE_ARGUMENT(TWeakPtr<class SUnicodeBrowserWidget>, UnicodeBrowser)
	SLATE_END_ARGS()

	DECLARE_DELEGATE_OneParam(FRangeClicked, EUnicodeBlockRange)
	FRangeClicked OnRangeClicked;

	DECLARE_DELEGATE_TwoParams(FRangeStateChanged, EUnicodeBlockRange, bool)
	FRangeStateChanged OnRangeStateChanged;
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	void SetRanges(TArray<EUnicodeBlockRange> RangesToSet);
	
	bool IsRangeChecked(EUnicodeBlockRange Range) const
	{
		return CheckboxIndices.Contains(Range) ? IsItemChecked(CheckboxIndices.FindChecked(Range)) : false;
	}
	
protected:	
	TMap<EUnicodeBlockRange const, int32 const> CheckboxIndices; // range <> SUbCheckBoxList index

	void UpdateRangeVisibility(int32 const Index);
	FReply RangeClicked(EUnicodeBlockRange const BlockRange) const;
};
