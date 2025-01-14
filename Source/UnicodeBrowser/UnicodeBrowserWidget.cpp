// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SCheckBoxList.h"
#include "SSimpleButton.h"
#include "SlateOptMacros.h"

#include "Components/VerticalBox.h"

#include "Editor/PropertyEditor/Private/Presentation/PropertyEditor/PropertyEditor.h"

#include "Fonts/FontMeasure.h"
#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "UnicodeBrowser/Widgets/SUbCheckboxList.h"
#include "UnicodeBrowser/Widgets/UbSimpleExpander.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SUbCheckBoxList> SUnicodeBrowserWidget::MakeRangeSelector()
{
	auto CheckBoxList = SNew(SUbCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);
	for (auto const& Range : Ranges)
	{
		auto ItemWidget = SNew(SSimpleButton)
			.Text(Range.DisplayName)
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
		auto const Index = RangeIndices[BlockRange];
		if (!RangeWidgets.IsValidIndex(Index)) return FReply::Unhandled();
		auto const RangeWidget = RangeWidgets[Index];
		RangeScrollbox->ScrollDescendantIntoView(RangeWidget);
		return FReply::Handled();
	}
	return FReply::Unhandled();
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
	auto const NumCols = Options->NumCols;
	auto FilteredRows = Rows.FilterByPredicate(
		[Range](TSharedPtr<FUnicodeBrowserRow> const& Row)
		{
			return Row->BlockRange == Range.Index;
		}
	);
	GridPanel->SetMinDesiredSlotHeight(Options->Font.Size * 2);
	GridPanel->SetMinDesiredSlotWidth(Options->Font.Size * 2);
	for (int32 i = 0; i < FilteredRows.Num(); ++i)
	{
		auto const Row = FilteredRows[i];
		auto Slot = GridPanel->AddSlot(i % NumCols, i / NumCols);
		TSharedPtr<STextBlock> TextBlock;
		if (!RowWidgetTextCache.Contains(Row->CharCode))
		{
			TextBlock = SNew(STextBlock)
			.Visibility(EVisibility::Visible)
			.OnDoubleClicked(this, &SUnicodeBrowserWidget::OnCharacterClicked, Row->Character)
			.Font(this, &SUnicodeBrowserWidget::GetFont)
			.Justification(ETextJustify::Center)
			.IsEnabled(true)
			.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: %d. Double-Click to copy: %s."), Row->CharCode, *Row->Character)))
			.Text(FText::FromString(FString::Printf(TEXT("%s"), *Row->Character)));
			RowWidgetTextCache.Add(Row->CharCode, TextBlock.ToSharedRef());
		} else
		{
			TextBlock = RowWidgetTextCache[Row->CharCode];
		}
		
		Slot
		[
			TextBlock.ToSharedRef()
		];
	}
}

TSharedPtr<SWidget> SUnicodeBrowserWidget::MakeRangeWidget(FUnicodeBlockRange const Range) const
{
	auto const GridPanel = SNew(SUniformGridPanel)
		.SlotPadding(FMargin(6.f, 4.f));
	Options->OnNumColsChanged.AddSPLambda(GridPanel.ToSharedPtr().Get(),
		[GridPanel, Range, this]()
		{
			GridPanel->ClearChildren();
			RebuildGridColumns(Range, GridPanel);
		}
	);

	RebuildGridColumns(Range, GridPanel);

	return SNew(SUbSimpleExpander)
		.Visibility_Lambda(
			[RangeIndex = Range.Index, this]()
			{
				if (!RangeSelector->GetNumCheckboxes()) return EVisibility::Collapsed;
				return RangeSelector->IsItemChecked(RangeIndices[RangeIndex]) ? EVisibility::Visible : EVisibility::Collapsed;
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

TSharedRef<IDetailsView> UUnicodeBrowserOptions::MakePropertyEditor(UUnicodeBrowserOptions* Options)
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bShowModifiedPropertiesOption = false;
	DetailsViewArgs.bShowScrollBar = false;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bShowObjectLabel = false;
	DetailsViewArgs.bLockable = false;
	auto FontDetailsView = PropertyEditor.CreateDetailView(DetailsViewArgs);
	FontDetailsView->SetObject(Options);
	return FontDetailsView;
}

FReply SUnicodeBrowserWidget::OnOnlySymbolsClicked()
{
	RangeSelector->UncheckAll();
	for (auto const SymbolRange : UnicodeBrowser::GetSymbolRanges())
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
	bool const bInit = Rows.Num() > 0;
	if (!bInit)
	{
		Ranges = UnicodeBrowser::GetUnicodeBlockRanges();
		PopulateSupportedCharacters();
		RangeWidgets.Reset();
		RangeIndices.Reset();
		RangeWidgets.Reserve(Ranges.Num());
		RangeIndices.Reserve(Ranges.Num());
	}
	RangeSelector = MakeRangeSelector();
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
			.MaxWidth(300)
			.HAlign(HAlign_Right)
			[

				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					FontDetailsView.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Only Symbols")))
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
			]
		]
	];

	if (!bInit)
	{
		for (auto const& Range : Ranges)
		{
			auto RangeWidget = MakeRangeWidget(Range);
			RangeWidgets.Add(RangeWidget);
		}
		OnOnlySymbolsClicked();
		for (auto const& RangeWidget : RangeWidgets)
		{
			RangeScrollbox->AddSlot()
			[
				RangeWidget.ToSharedRef()
			];
		}
	}
}

void SUnicodeBrowserWidget::PopulateSupportedCharacters()
{
	FSlateFontInfo const FontInfo = Options->Font;
	TSharedRef<FSlateFontMeasure> const FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
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
