// all rights reserved


#include "SUnicodeBlockRangeSelector.h"

#include "SlateOptMacros.h"
#include "SSimpleButton.h"
#include "UnicodeBrowser/UnicodeBrowserWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeBlockRangeSelector::Construct(const FArguments& InArgs)
{
	SUbCheckBoxList::Construct(SUbCheckBoxList::FArguments()
		.ItemHeaderLabel(INVTEXT("Unicode Block Ranges"))
		.IncludeGlobalCheckBoxInHeaderRow(true)
	);

	if(!InArgs._UnicodeBrowser.IsValid())
		return;		
	
	CheckboxIndices.Reset();
	CheckboxIndices.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());

	OnItemCheckStateChanged.BindSP(this, &SUnicodeBlockRangeSelector::UpdateRangeVisibility);
	
	for (FUnicodeBlockRange const &Range : UnicodeBrowser::GetUnicodeBlockRanges())
	{
		auto ItemWidget = SNew(SSimpleButton)
			.Text_Lambda([UnicodeBrowser = InArgs._UnicodeBrowser, Range = Range]()
			{
				if(!UnicodeBrowser.IsValid())
					return FText::GetEmpty();
				
				int32 const Num = UnicodeBrowser.Pin().Get()->Rows.Contains(Range.Index) ? UnicodeBrowser.Pin().Get()->Rows.FindChecked(Range.Index).Num() : 0;
				return FText::FromString(FString::Printf(TEXT("%s (%d)"), *Range.DisplayName.ToString(), Num));
			})
			.ToolTipText(
				FText::FromString(
					FString::Printf(
						TEXT("%s: Range U+%-06.04X Codes U+%-06.04X - U+%-06.04X"),
						*Range.DisplayName.ToString(),
						Range.Index,
						Range.Range.GetLowerBoundValue(),
						Range.Range.GetUpperBoundValue()
					)
				)
			)
			.OnClicked(this, &SUnicodeBlockRangeSelector::RangeClicked, Range.Index);

		int32 Index = AddItem(ItemWidget, false);
		CheckboxIndices.Add(Range.Index, Index);
	}
}

void SUnicodeBlockRangeSelector::SetRanges(TArray<EUnicodeBlockRange> RangesToSet)
{
	// update all checkboxes and keep the amount of state updates as low as possible to avoid redraw
	for (auto const &Range : UnicodeBrowser::GetUnicodeBlockRanges())
	{
		if (!CheckboxIndices.Contains(Range.Index))
		{
			UE_LOG(LogTemp, Warning, TEXT("No checkbox index found for range %d."), Range.Index);
			continue;
		}

		SetItemChecked(CheckboxIndices[Range.Index], RangesToSet.Contains(Range.Index) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}


void SUnicodeBlockRangeSelector::UpdateRangeVisibility(int32 const Index)
{
	auto const RangeFound = CheckboxIndices.FindKey(Index);
	if (!RangeFound) return;
	auto const Range = *RangeFound;
	OnRangeStateChanged.ExecuteIfBound(Range, IsItemChecked(Index));	
}

FReply SUnicodeBlockRangeSelector::RangeClicked(EUnicodeBlockRange const BlockRange) const
{
	ensureAlways(CheckboxIndices.Contains(BlockRange));
	OnRangeClicked.ExecuteIfBound(BlockRange);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
