// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SCheckBoxList.h"
#include "SlateOptMacros.h"

#include "Components/VerticalBox.h"

#include "Fonts/FontMeasure.h"
#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "UnicodeBrowser/Widgets/UbSimpleExpander.h"

#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SCheckBoxList> SUnicodeBrowserWidget::MakeRangeSelector() const
{
	auto CheckBoxList = SNew(SCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);
	auto const Ranges = FUnicodeBlockRange::GetUnicodeBlockRanges();
	for (auto const& Range : Ranges)
	{
		CheckBoxList->AddItem(Range.DisplayName, false);
	}
	return CheckBoxList;
}

TSharedPtr<SWidget> SUnicodeBrowserWidget::MakeRangeWidget(EUnicodeBlockRange BlockRange, uint32 const NumCols, TSharedPtr<SCheckBoxList> RangeSelector) const
{
	auto const Range = FUnicodeBlockRange::GetUnicodeBlockRange(BlockRange);
	auto const GridPanel = SNew(SUniformGridPanel)
		.SlotPadding(FMargin(4.f));

	auto FilteredRows = Rows.FilterByPredicate(
		[BlockRange](TSharedPtr<FUnicodeBrowserRow> Row)
		{
			return Row->BlockRange == BlockRange;
		}
	);
	for (int32 i = 0; i < FilteredRows.Num(); ++i)
	{
		auto const Row = FilteredRows[i];
		auto Slot = GridPanel->AddSlot(i % NumCols, i / NumCols);
		Slot
		[
			SNew(STextBlock)
			.Visibility(EVisibility::Visible)
			.OnDoubleClicked_Lambda(
				[Row](FGeometry const&, FPointerEvent const&)
				{
					FPlatformApplicationMisc::ClipboardCopy(*Row->Character);
					return FReply::Handled();
				}
			)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
			.Justification(ETextJustify::Center)
			.IsEnabled(true)
			.Text(FText::FromString(FString::Printf(TEXT("%s"), *Row->Character)))
		];
	}
	return SNew(SUbSimpleExpander)
		.Visibility_Lambda(
			[BlockRange, RangeSelector]()
			{
				if (!RangeSelector->GetNumCheckboxes()) return EVisibility::Collapsed;
				return RangeSelector->IsItemChecked(UnicodeBrowser::GetRangeIndex(BlockRange)) ? EVisibility::Visible : EVisibility::Collapsed;
			}
		)
		.Header()
		[
			SNew(STextBlock)
			.Text(Range.DisplayName)
		]
		.Body()
		[
			GridPanel
		];
}

void SUnicodeBrowserWidget::Construct(FArguments const& InArgs)
{
	if (Rows.Num() == 0)
	{
		PopulateSupportedCharacters();
	}
	auto const NumCols = 32;
	auto const RangeSelector = MakeRangeSelector();
	auto const Ranges = FUnicodeBlockRange::GetUnicodeBlockRanges();
	TArray<TSharedPtr<SWidget>> RangeWidgets;
	RangeWidgets.Reserve(Ranges.Num());
	for (auto const& Range : Ranges)
	{
		if (!Rows.FindByPredicate(
			[Range](auto Row)
			{
				return Row->BlockRange == Range.Index;
			}
		))
		{
			continue;
		}
		auto RangeWidget = MakeRangeWidget(Range.Index, NumCols, RangeSelector);
		RangeWidgets.Add(RangeWidget);
	}
	TSharedPtr<SScrollBox> RangeScrollbox;
	ChildSlot
	[

		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1)
		.FillContentHeight(1)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.FillContentWidth(1)
			.MinWidth(500)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				.FillContentHeight(1)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					SAssignNew(RangeScrollbox, SScrollBox)
				]
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(200)
			.HAlign(HAlign_Right)
			[

				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				.FillContentHeight(1)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					RangeSelector.ToSharedRef()
				]
			]
		]
	];

	for (auto const& RangeWidget : RangeWidgets)
	{
		RangeScrollbox->AddSlot()
		[
			RangeWidget.ToSharedRef()
		];
	}
}

void SUnicodeBrowserWidget::PopulateSupportedCharacters()
{
	FSlateFontInfo const FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 12);
	TSharedRef<FSlateFontMeasure> const FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	auto const Ranges = FUnicodeBlockRange::GetUnicodeBlockRanges();
	for (auto const& Range : Ranges)
	{
		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			FString UnicodeString;
			if (FUnicodeChar::CodepointToString(CharCode, UnicodeString))
			{
				if (FontMeasureService->Measure(UnicodeString, FontInfo).X > 0)
				{
					Rows.Add(MakeShared<FUnicodeBrowserRow>(CharCode, UnicodeString, Range.Index));
				}
			}
		}
	}
}

FString SUnicodeBrowserWidget::GetUnicodeCharacterName(int32 const CharCode)
{
	UChar32 const uChar = static_cast<UChar32>(CharCode);
	UErrorCode errorCode = U_ZERO_ERROR;
	// uint32 bufferSize = 256;
	// char *name= (char *)uprv_malloc(bufferSize*sizeof(char));
	// (char *)uprv_malloc(bufferSize*sizeof(char));
	// char name[256];
	char* name = new char[256];

	// Get the Unicode character name using ICU
	int32_t const length = u_charName(uChar, U_CHAR_NAME_ALIAS, name, 256, &errorCode);
	FString Result;
	if (U_SUCCESS(errorCode) && length > 0)
	{
		Result = FString(name);
	}
	else
	{
		Result = FString::Printf(TEXT("Unknown %hs"), u_errorName(errorCode));
	}
	delete[] name;
	return Result;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
