// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SCheckBoxList.h"
#include "SSimpleButton.h"
#include "SlateOptMacros.h"

#include "Components/VerticalBox.h"

#include "Editor/PropertyEditor/Private/Presentation/PropertyEditor/PropertyEditor.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "Modules/ModuleManager.h"

#include "UnicodeBrowser/Widgets/SUbCheckBoxList.h"

#include "Widgets/SInvalidationPanel.h"
#include "Widgets/UnicodeCharacterInfo.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

THIRD_PARTY_INCLUDES_START
#include <unicode/uchar.h>
#include <unicode/unistr.h>
THIRD_PARTY_INCLUDES_END

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TOptional<EUnicodeBlockRange> UnicodeBrowser::GetUnicodeBlockRangeFromChar(int32 const CharCode)
{
	for (auto const& BlockRange : FUnicodeBlockRange::GetUnicodeBlockRanges())
	{
		if (BlockRange.Range.Contains(CharCode))
		{
			return BlockRange.Index;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No Unicode block range found for character code 0x%04X: %s"), CharCode, *FString::Chr(CharCode));
	return {};
}

TArrayView<FUnicodeBlockRange const> UnicodeBrowser::GetUnicodeBlockRanges()
{
	auto BlockRange = FUnicodeBlockRange::GetUnicodeBlockRanges();
	BlockRange.StableSort(
		[](FUnicodeBlockRange const& A, FUnicodeBlockRange const& B)
		{
			return A.DisplayName.CompareTo(B.DisplayName) < 0;
		}
	);
	return BlockRange;
}

int32 UnicodeBrowser::GetRangeIndex(EUnicodeBlockRange BlockRange)
{
	return GetUnicodeBlockRanges().IndexOfByPredicate(
		[BlockRange](FUnicodeBlockRange const& Range)
		{
			return Range.Index == BlockRange;
		}
	);
}

TSharedPtr<SUbCheckBoxList> SUnicodeBrowserWidget::MakeRangeSelector()
{
	auto CheckBoxList = SNew(SUbCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);

	for (auto const& Range : Ranges)
	{
		int32 const Num = Rows.Contains(Range.Index) ? Rows.FindChecked(Range.Index).Num() : 0;

		auto ItemWidget = SNew(SSimpleButton)
			.Text(FText::FromString(FString::Printf(TEXT("%s (%d)"), *Range.DisplayName.ToString(), Num)))
			.ToolTipText(
				FText::FromString(FString::Printf(TEXT("%s: Range 0x%04X Codes 0x%04X - 0x%04X"), *Range.DisplayName.ToString(), Range.Index, Range.Range.GetLowerBoundValue(), Range.Range.GetUpperBoundValue()))
			)
			.OnClicked(this, &SUnicodeBrowserWidget::OnRangeClicked, Range.Index);

		int32 Index = CheckBoxList->AddItem(ItemWidget, false);
		RangeIndices.Add(Range.Index, Index);
	}

	return CheckBoxList;
}

FReply SUnicodeBrowserWidget::OnRangeClicked(EUnicodeBlockRange const BlockRange) const
{
	if (RangeScrollbox.IsValid())
	{
		if (!RangeIndices.Contains(BlockRange)) return FReply::Unhandled();
		if (!RangeWidgets.Contains(BlockRange)) return FReply::Unhandled();
		auto const RangeWidget = RangeWidgets.FindChecked(BlockRange);
		RangeScrollbox->ScrollDescendantIntoView(RangeWidget);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply SUnicodeBrowserWidget::OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const
{
	if (CurrentRow == Row) return FReply::Unhandled();
	CurrentRow = Row;
	CurrentCharacterView->SetText(FText::FromString(CurrentRow->Character));
	CurrentCharacterView->SetToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), CurrentRow->Codepoint, *CurrentRow->Character)));
	return FReply::Handled();
}

FReply SUnicodeBrowserWidget::OnCurrentCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent) const
{
	return OnCharacterClicked(Geometry, PointerEvent, CurrentRow->Character);
}

FReply SUnicodeBrowserWidget::OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const
{
	FPlatformApplicationMisc::ClipboardCopy(*Character);
	return FReply::Handled();
}

FSlateFontInfo SUnicodeBrowserWidget::GetFont() const
{
	if (!Options) return FCoreStyle::GetDefaultFontStyle("Regular", 18);
	return Options->Font;
}

void SUnicodeBrowserWidget::RebuildGridColumns(FUnicodeBlockRange const Range, TSharedRef<SUniformGridPanel> const GridPanel) const
{
	GridPanel->ClearChildren();

	if(!Rows.Contains(Range.Index))
	{
		return;	
	}	
	
	auto const NumCols = Options->NumCols;
	GridPanel->SetMinDesiredSlotHeight(Options->Font.Size * 2);
	GridPanel->SetMinDesiredSlotWidth(Options->Font.Size * 2);

	auto RowEntries = Rows.FindChecked(Range.Index);
	
	for (int32 i = 0; i < RowEntries.Num(); ++i)
	{
		auto const Row = RowEntries[i];
		auto Slot = GridPanel->AddSlot(i % NumCols, i / NumCols);
		TSharedPtr<SBorder> TextBlock;
		if (!RowWidgetTextCache.Contains(Row->Codepoint))
		{
			TextBlock = SNew(SBorder)
				.BorderImage(nullptr)
				.OnMouseMove(this, &SUnicodeBrowserWidget::OnCharacterMouseMove, Row)
				[
					SNew(STextBlock)
					.Visibility(EVisibility::Visible)
					.OnDoubleClicked(this, &SUnicodeBrowserWidget::OnCharacterClicked, Row->Character)
					.Font(this, &SUnicodeBrowserWidget::GetFont)
					.Justification(ETextJustify::Center)
					.IsEnabled(true)
					.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), Row->Codepoint, *Row->Character)))
					.Text(FText::FromString(FString::Printf(TEXT("%s"), *Row->Character)))
				];

			RowWidgetTextCache.Add(Row->Codepoint, TextBlock.ToSharedRef());
		}
		else
		{
			TextBlock = RowWidgetTextCache[Row->Codepoint];
		}

		Slot
		[
			TextBlock.ToSharedRef()
		];
	}
}


void SUnicodeBrowserWidget::MakeRangeWidget(FUnicodeBlockRange const Range)
{
	auto const GridPanel = SNew(SUniformGridPanel)
		.SlotPadding(FMargin(6.f, 4.f));
	
	RebuildGridColumns(Range, GridPanel);

	auto RangeWidget = SNew(SExpandableArea)
		.Visibility_Lambda(
			[RangeIndex = Range.Index, this]()
			{
				if (!RangeSelector->GetNumCheckboxes()) return EVisibility::Collapsed;
				return RangeSelector->IsItemChecked(RangeIndices[RangeIndex]) ? EVisibility::Visible : EVisibility::Collapsed;
			}
		)
		.HeaderPadding(FMargin(2, 4))
		.HeaderContent()
		[
			SNew(STextBlock)
			.Text(Range.DisplayName)
		]
		.BodyContent()
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				GridPanel
			]
		];

	RangeWidgets.Add(Range.Index, RangeWidget);
	RangeWidgetsGrid.Add(Range.Index, GridPanel);
}

FReply SUnicodeBrowserWidget::OnOnlySymbolsClicked()
{
	RangeSelector->UncheckAll();
	for (auto const SymbolRange : UnicodeBrowser::SymbolRanges)
	{
		int32 const Index = RangeIndices[SymbolRange];
		RangeSelector->SetItemChecked(Index, ECheckBoxState::Checked);
	}
	return FReply::Handled();
}

void SUnicodeBrowserWidget::Construct(FArguments const& InArgs)
{
	if (!Options)
	{
		Options = NewObject<UUnicodeBrowserOptions>();
		Options->Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		FontDetailsView = UUnicodeBrowserOptions::MakePropertyEditor(Options);		
	}

	// idk if Construct may be called multiple times, so this will prevent several 
	if(!Options->OnChanged.IsBound()){
		Options->OnChanged.AddRaw(this,	&SUnicodeBrowserWidget::UpdateFromFont);
	}
	
	bool const bInit = !Rows.IsEmpty();
	if (!bInit)
	{
		Ranges = UnicodeBrowser::GetUnicodeBlockRanges();
		PopulateSupportedCharacters();
		RangeWidgets.Reset();
		RangeWidgetsGrid.Reset();
		RangeIndices.Reset();
		RangeWidgets.Reserve(Ranges.Num());
		RangeIndices.Reserve(Ranges.Num());
		RangeWidgetsGrid.Reserve(Ranges.Num());
	}
	RangeSelector = MakeRangeSelector();
	CurrentRow = MakeShared<FUnicodeBrowserRow>(0, EUnicodeBlockRange::ControlCharacter); // create a dummy for the preview until the user highlights a character
	
	ChildSlot
	[

		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1)
		.FillContentHeight(1)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			.ResizeMode(ESplitterResizeMode::Fill)
			+ SSplitter::Slot()
			.SizeRule(SSplitter::FractionOfParent)
			.Value(0.7)
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
			+ SSplitter::Slot()
			.SizeRule(SSplitter::FractionOfParent)
			.Value(0.3)
			[

				SNew(SSplitter)
				.Orientation(Orient_Vertical)
				.ResizeMode(ESplitterResizeMode::Fill)
				+ SSplitter::Slot()
				.SizeRule(SSplitter::FractionOfParent)
				.Value(0.25)
				.MinSize(50)
				[

					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFit)
					[
						SAssignNew(CurrentCharacterView, STextBlock)
						.Visibility(EVisibility::Visible)
						.OnDoubleClicked(this, &SUnicodeBrowserWidget::OnCurrentCharacterClicked)
						.Font(this, &SUnicodeBrowserWidget::GetFont)
						.Justification(ETextJustify::Center)
						.IsEnabled(true)
						.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), CurrentRow->Codepoint, *CurrentRow->Character)))
						.Text(FText::FromString(FString::Printf(TEXT("%s"), *CurrentRow->Character)))
					]
				]
				+ SSplitter::Slot()
				.SizeRule(SSplitter::FractionOfParent)
				.Value(0.75)
				[

					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					[
						SNew(SExpandableArea)
						.HeaderPadding(FMargin(2, 4))
						.HeaderContent()
						[
							SNew(STextBlock)
							.Font(FAppStyle::Get().GetFontStyle("DetailsView.CategoryFontStyle"))
							.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
							.Text(FText::FromString(TEXT("Character Info")))
						]
						.BodyContent()
						[
							SNew(SUnicodeCharacterInfo)
							.Row_Lambda([this]() { return CurrentRow; })
						]

					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Fill)
					[
						FontDetailsView.ToSharedRef()
					]
					+ SVerticalBox::Slot()
					.FillHeight(1)
					.FillContentHeight(1)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					[

						SNew(SExpandableArea)
						.HeaderPadding(FMargin(2, 4))
						.HeaderContent()
						[
							SNew(STextBlock)
							.Font(FAppStyle::Get().GetFontStyle("DetailsView.CategoryFontStyle"))
							.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
							.Text(FText::FromString(TEXT("Unicode Block Ranges")))
						]
						.BodyContent()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							.VAlign(VAlign_Center)
							.Padding(0, 4)
							.HAlign(HAlign_Center)
							[
								SNew(SButton)
								.Text(FText::FromString(TEXT("Preset: Only Symbol Blocks")))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.OnClicked(this, &SUnicodeBrowserWidget::OnOnlySymbolsClicked)
							]
							+ SVerticalBox::Slot()
							.FillHeight(1)
							.FillContentHeight(1)
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							[
								SNew(SScrollBox)
								.Orientation(Orient_Vertical)
								+ SScrollBox::Slot()
								.FillSize(1.f)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.AutoHeight()
									[
										RangeSelector.ToSharedRef()
									]
								]
							]
						]
					]
				]
			]
		]
	];

	if (!bInit)
	{
		for (auto const& Range : Ranges)
		{
			MakeRangeWidget(Range);			
		}
		OnOnlySymbolsClicked();
		for (auto const& RangeWidget : RangeWidgets)
		{
			RangeScrollbox->AddSlot()
			[
				RangeWidget.Value.ToSharedRef()
			];
		}
	}
}

void SUnicodeBrowserWidget::UpdateFromFont(struct FPropertyChangedEvent* PropertyChangedEvent)
{
	// rebuild the character list
	PopulateSupportedCharacters();

	// rebuild the grid
	for (auto const& Range : Ranges)
	{
		if(auto GridPanel = RangeWidgetsGrid.Find(Range.Index))
		{
			RebuildGridColumns(Range, GridPanel->ToSharedRef());
		}
	}

	// set range visibility automatic
	for (auto const& Range : Ranges)
	{
		if(RangeIndices.Contains(Range.Index)){
			const bool bRangeHasCharacters = Rows.Contains(Range.Index) ? !Rows.FindChecked(Range.Index).IsEmpty() : false;
			RangeSelector.Get()->SetItemChecked(RangeIndices.FindChecked(Range.Index), bRangeHasCharacters ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
		}
	}
}

void SUnicodeBrowserWidget::PopulateSupportedCharacters()
{
	Rows.Empty();
	Rows.Reserve(Ranges.Num());
	
	for (auto const& Range : Ranges)
	{
		Rows.Add(Range.Index);
		TArray<TSharedPtr<FUnicodeBrowserRow>> &RangeArray = Rows.FindChecked(Range.Index);
		
		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			auto Row = MakeShared<FUnicodeBrowserRow>(CharCode, Range.Index, &Options->Font);

			if(Options->bCacheCharacterMetaOnLoad)
			{
				Row->GetMeasurements();
				Row->CanLoadCodepoint();
			}
			
			if(Options->bShowMissing || Row->CanLoadCodepoint())
			{
				RangeArray.Add(Row);
			}
		}
	}
}

FString SUnicodeBrowserWidget::GetUnicodeCharacterName(int32 const CharCode)
{
	UChar32 const uChar = static_cast<UChar32>(CharCode);
	UErrorCode errorCode = U_ZERO_ERROR;
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