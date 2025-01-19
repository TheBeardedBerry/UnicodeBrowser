// all rights reserved


#include "SUnicodeBlockRangeSelector.h"

#include "SlateOptMacros.h"
#include "SSimpleButton.h"
#include "UnicodeBrowser/UnicodeBrowserWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeBlockRangeSelector::Construct(const FArguments& InArgs)
{
	if(!InArgs._UnicodeBrowser.IsValid())
		return;

	UnicodeBrowser = InArgs._UnicodeBrowser;
	
	CheckboxIndices.Reset();
	CheckboxIndices.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());

	CheckBoxList = SNew(SUbCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);
	
	CheckBoxList->OnItemCheckStateChanged.BindSP(this, &SUnicodeBlockRangeSelector::UpdateRangeVisibility);
	
	for (FUnicodeBlockRange const &Range : UnicodeBrowser::GetUnicodeBlockRanges())
	{
		auto ItemWidget = SNew(SSimpleButton)
			.Text_Lambda([UnicodeBrowser = UnicodeBrowser, Range = Range]()
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

		int32 Index = CheckBoxList->AddItem(ItemWidget, false);		
		CheckboxIndices.Add(Range.Index, Index);
	}

	UnicodeBrowser.Pin()->OnFontChanged.AddSP(this, &SUnicodeBlockRangeSelector::UpdateRowVisibility);

	ChildSlot[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(CheckBox_HideEmptyRanges, SCheckBox)
			.IsChecked(true)
			.OnCheckStateChanged_Lambda([this](ECheckBoxState State)
			{
				UpdateRowVisibility(nullptr);
			})
			[
				SNew(STextBlock)
				.Text(INVTEXT("hide empty ranges"))
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(1, 5))
		]
		+SVerticalBox::Slot()
		[
			CheckBoxList.ToSharedRef()
		]
	];	
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

		CheckBoxList->SetItemChecked(CheckboxIndices[Range.Index], RangesToSet.Contains(Range.Index) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}


void SUnicodeBlockRangeSelector::UpdateRangeVisibility(int32 const Index)
{
	auto const RangeFound = CheckboxIndices.FindKey(Index);
	if (!RangeFound) return;
	auto const Range = *RangeFound;
	OnRangeStateChanged.ExecuteIfBound(Range, CheckBoxList->IsItemChecked(Index));	
}


void SUnicodeBlockRangeSelector::UpdateRowVisibility(FSlateFontInfo* FontInfo)
{
	if(!UnicodeBrowser.IsValid())
		return;
		
	bool const bHideEmptyRanges = CheckBox_HideEmptyRanges->IsChecked();
	for(auto &[Range, ItemIndex] : CheckboxIndices)
	{
		// we use the raw character count, otherwise the resulting changes may be too confusing for the user
		int32 const CharacterCount = UnicodeBrowser.Pin().Get()->RowsRaw.Contains(Range) ? UnicodeBrowser.Pin().Get()->RowsRaw.FindChecked(Range).Num() : 0;
		CheckBoxList->Items[ItemIndex].Get().bIsVisible = !bHideEmptyRanges || CharacterCount > 0;
	}

	CheckBoxList->UpdateItems();
}

FReply SUnicodeBlockRangeSelector::RangeClicked(EUnicodeBlockRange const BlockRange) const
{
	ensureAlways(CheckboxIndices.Contains(BlockRange));
	OnRangeClicked.ExecuteIfBound(BlockRange);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
