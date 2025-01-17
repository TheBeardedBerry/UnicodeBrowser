// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SSimpleButton.h"
#include "SlateOptMacros.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "Modules/ModuleManager.h"

#include "UnicodeBrowser/Widgets/SUbCheckBoxList.h"
#include "Widgets/SUnicodeRangeWidget.h"

#include "Widgets/UnicodeCharacterInfo.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"



BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SUbCheckBoxList> SUnicodeBrowserWidget::MakeBlockRangeSelector()
{
	auto CheckBoxList = SNew(SUbCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);

	for (auto const& Range : Ranges)
	{
		auto ItemWidget = SNew(SSimpleButton)
			.Text_Lambda([this, Range]()
			{
				int32 const Num = Rows.Contains(Range.Index) ? Rows.FindChecked(Range.Index).Num() : 0;
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
			.OnClicked(this, &SUnicodeBrowserWidget::OnRangeClicked, Range.Index);

		int32 Index = CheckBoxList->AddItem(ItemWidget, false);
		CheckboxIndices.Add(Range.Index, Index);
	}

	return CheckBoxList;
}

FReply SUnicodeBrowserWidget::OnRangeClicked(EUnicodeBlockRange const BlockRange) const
{
	if (RangeScrollbox.IsValid())
	{
		if (!CheckboxIndices.Contains(BlockRange)) return FReply::Unhandled();
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

FSlateFontInfo SUnicodeBrowserWidget::GetFontInfo() const
{
	if (!Options) return FCoreStyle::GetDefaultFontStyle("Regular", 18);
	return Options->FontInfo;
}

void SUnicodeBrowserWidget::RebuildGrid(FUnicodeBlockRange const Range, TSharedRef<SUniformGridPanel> const GridPanel) const
{
	GridPanel->ClearChildren();

	if (!Rows.Contains(Range.Index)) return;

	auto const NumCols = Options->NumCols;
	GridPanel->SetMinDesiredSlotHeight(Options->FontInfo.Size * 2);
	GridPanel->SetMinDesiredSlotWidth(Options->FontInfo.Size * 2);

	auto RowEntries = Rows.FindChecked(Range.Index);
	// pad the grid with empty slots so that the grid is always filled with a uniform number of columns
	auto const NumEntries = FMath::Max(RowEntries.Num(), NumCols);
	for (int32 i = 0; i < NumEntries; ++i)
	{
		auto Slot = GridPanel->AddSlot(i % NumCols, i / NumCols);
		// empty slot
		if (!RowEntries.IsValidIndex(i)) continue;

		auto const Row = RowEntries[i];
		TSharedPtr<SCompoundWidget> GridCell;
		if (!RowCellWidgetCache.Contains(Row->Codepoint))
		{
			GridCell = SNew(SBorder)
				.BorderImage(nullptr)
				.OnMouseMove(this, &SUnicodeBrowserWidget::OnCharacterMouseMove, Row)
				[
					SNew(STextBlock)
					.Font(this, &SUnicodeBrowserWidget::GetFontInfo)
					.IsEnabled(true)
					.Justification(ETextJustify::Center)
					.Text(FText::FromString(FString::Printf(TEXT("%s"), *Row->Character)))
					.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), Row->Codepoint, *Row->Character)))
					.OnDoubleClicked(this, &SUnicodeBrowserWidget::OnCharacterClicked, Row->Character)
				];

			RowCellWidgetCache.Add(Row->Codepoint, GridCell.ToSharedRef());
		}
		else
		{
			GridCell = RowCellWidgetCache[Row->Codepoint];
		}

		Slot
		[
			GridCell.ToSharedRef()
		];
	}
}

TSharedPtr<SExpandableArea> SUnicodeBrowserWidget::MakeBlockRangesSidebar()
{
	return SNew(SExpandableArea)
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
			.HAlign(HAlign_Fill)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Preset: Blocks with Characters")))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.OnClicked(this, &SUnicodeBrowserWidget::OnOnlyBlocksWithCharactersClicked)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Center)
			.Padding(0, 4)
			.HAlign(HAlign_Fill)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Preset: Symbols")))
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
				RangeSelector.ToSharedRef()
			]
		];
}

FReply SUnicodeBrowserWidget::OnOnlySymbolsClicked()
{
	RangeSelector->UncheckAll();
	for (auto const SymbolRange : UnicodeBrowser::SymbolRanges)
	{
		if (!CheckboxIndices.Contains(SymbolRange))
		{
			UE_LOG(LogTemp, Warning, TEXT("No checkbox index found for symbol range %d."), SymbolRange);
			continue;
		}
		int32 const CheckboxIndex = CheckboxIndices[SymbolRange];
		RangeSelector->SetItemChecked(CheckboxIndex, ECheckBoxState::Checked);
	}
	return FReply::Handled();
}

FReply SUnicodeBrowserWidget::OnOnlyBlocksWithCharactersClicked() const
{
	SelectAllRangesWithCharacters();
	return FReply::Handled();
}

void SUnicodeBrowserWidget::UpdateRangeVisibility(int32 const Index)
{
	auto const RangeFound = CheckboxIndices.FindKey(Index);
	if (!RangeFound) return;
	auto const Range = *RangeFound;
	if (!RangeWidgets.Contains(Range)) return;
	auto const RangeWidget = RangeWidgets.FindChecked(Range);
	RangeWidget->SetVisibility(RangeSelector->IsItemChecked(Index) ? EVisibility::Visible : EVisibility::Collapsed);
}

void SUnicodeBrowserWidget::MarkDirty()
{
	// schedule an update on the next tick
	SetCanTick(true);
	bDirty = true;
}

void SUnicodeBrowserWidget::Tick(FGeometry const& AllottedGeometry, double const InCurrentTime, float const InDeltaTime)
{
	if (bDirty)
	{
		if (IsConstructed()) // wait for the widget to be constructed before updating
		{
			bDirty = false;
			SetCanTick(false);
			Update();
		}
	}
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SUnicodeBrowserWidget::Construct(FArguments const& InArgs)
{
	if (!Options)
	{
		Options = NewObject<UUnicodeBrowserOptions>();
		Options->FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
		FontDetailsView = UUnicodeBrowserOptions::MakePropertyEditor(Options);
	}

	Options->OnChanged.Remove(OnOptionsChangedHandle);
	OnOptionsChangedHandle = Options->OnChanged.AddSP(this, &SUnicodeBrowserWidget::UpdateFromFont);

	bool const bIsInitialized = !Rows.IsEmpty();
	if (!bIsInitialized)
	{
		Ranges = UnicodeBrowser::GetUnicodeBlockRanges();
		RangeWidgets.Reset();
		CheckboxIndices.Reset();
		RangeWidgets.Reserve(Ranges.Num());
		CheckboxIndices.Reserve(Ranges.Num());
		// note character/font data isn't initialized until first tick after construction
		// this permits the browser panel to display earlier
		MarkDirty();
	}

	RangeSelector = MakeBlockRangeSelector();
	RangeSelector->OnItemCheckStateChanged.BindSP(this, &SUnicodeBrowserWidget::UpdateRangeVisibility);
	BlockRangesSidebar = MakeBlockRangesSidebar();
	// create a dummy for the preview until the user highlights a character
	CurrentRow = MakeShared<FUnicodeBrowserRow>(UnicodeBrowser::InvalidSubChar, EUnicodeBlockRange::Specials, &Options->FontInfo);

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
						.Font(this, &SUnicodeBrowserWidget::GetFontInfo)
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
					SNew(SScrollBox)
					.Orientation(Orient_Vertical)
					+ SScrollBox::Slot()
					.FillSize(1.f)
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
							BlockRangesSidebar.ToSharedRef()
						]
					]
				]
			]
		]
	];
}

void SUnicodeBrowserWidget::Update()
{
	bool const bIsInitialized = !Rows.IsEmpty();
	// rebuild the character list
	PopulateSupportedCharacters();

	if (!bIsInitialized)
	{
		// create the range widgets for the first time
		for (auto const& Range : Ranges)
		{
			RangeWidgets.Add(Range.Index,
				SNew(SUnicodeRangeWidget)
					.Range(Range)
					.Visibility(RangeSelector->IsItemChecked(CheckboxIndices[Range.Index]) ? EVisibility::Visible : EVisibility::Collapsed));
		}
		for (auto const& RangeWidget : RangeWidgets)
		{
			RangeScrollbox->AddSlot()
			[
				RangeWidget.Value.ToSharedRef()
			];
		}

		// default preset
		OnOnlySymbolsClicked();
	}

	// rebuild the grid
	for (auto const& Range : Ranges)
	{
		if (const auto RangeWidget = RangeWidgets.Find(Range.Index))
		{
			RebuildGrid(Range, RangeWidget->Get()->GetGridPanel());
		}
	}
}

void SUnicodeBrowserWidget::UpdateFromFont(struct FPropertyChangedEvent* PropertyChangedEvent)
{
	MarkDirty();
}

void SUnicodeBrowserWidget::SelectAllRangesWithCharacters() const
{
	RangeSelector.Get()->UncheckAll();
	for (auto const& Range : Ranges)
	{
		if (CheckboxIndices.Contains(Range.Index))
		{
			bool const bRangeHasCharacters = Rows.Contains(Range.Index) ? !Rows.FindChecked(Range.Index).IsEmpty() : false;
			RangeSelector.Get()->SetItemChecked(CheckboxIndices.FindChecked(Range.Index), bRangeHasCharacters ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
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
		TArray<TSharedPtr<FUnicodeBrowserRow>>& RangeArray = Rows.FindChecked(Range.Index);

		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			auto Row = MakeShared<FUnicodeBrowserRow>(CharCode, Range.Index, &Options->FontInfo);

			if (Options->bCacheCharacterMetaOnLoad)
			{
				Row->Preload();
			}

			if (Options->bShowMissing || Row->CanLoadCodepoint())
			{
				RangeArray.Add(Row);
			}
		}
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
