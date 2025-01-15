// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SCheckBoxList.h"
#include "SSimpleButton.h"
#include "SlateOptMacros.h"

#include "Algo/Count.h"

#include "Components/VerticalBox.h"

#include "Editor/PropertyEditor/Private/Presentation/PropertyEditor/PropertyEditor.h"

#include "Fonts/FontMeasure.h"
#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "Modules/ModuleManager.h"

#include "UnicodeBrowser/Widgets/SUbCheckBoxList.h"

#include "Widgets/SInvalidationPanel.h"
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
	UE_LOG(LogTemp, Warning, TEXT("No Unicode block range found for character code %d: %s"), CharCode, *FString::Chr(CharCode));
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

TArray<EUnicodeBlockRange> UnicodeBrowser::GetSymbolRanges()
{
	TArray<EUnicodeBlockRange> SymbolRanges;
	SymbolRanges.Add(EUnicodeBlockRange::Arrows);
	SymbolRanges.Add(EUnicodeBlockRange::BlockElements);
	SymbolRanges.Add(EUnicodeBlockRange::BoxDrawing);
	SymbolRanges.Add(EUnicodeBlockRange::CurrencySymbols);
	SymbolRanges.Add(EUnicodeBlockRange::Dingbats);
	SymbolRanges.Add(EUnicodeBlockRange::EmoticonsEmoji);
	SymbolRanges.Add(EUnicodeBlockRange::EnclosedAlphanumericSupplement);
	SymbolRanges.Add(EUnicodeBlockRange::EnclosedAlphanumerics);
	SymbolRanges.Add(EUnicodeBlockRange::GeneralPunctuation);
	SymbolRanges.Add(EUnicodeBlockRange::GeometricShapes);
	SymbolRanges.Add(EUnicodeBlockRange::MathematicalAlphanumericSymbols);
	SymbolRanges.Add(EUnicodeBlockRange::MathematicalOperators);
	SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousMathematicalSymbolsB);
	SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbols);
	SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbolsAndArrows);
	SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousSymbolsAndPictographs);
	SymbolRanges.Add(EUnicodeBlockRange::MiscellaneousTechnical);
	SymbolRanges.Add(EUnicodeBlockRange::NumberForms);
	SymbolRanges.Add(EUnicodeBlockRange::SupplementalSymbolsAndPictographs);
	SymbolRanges.Add(EUnicodeBlockRange::TransportAndMapSymbols);
	SymbolRanges.Add(EUnicodeBlockRange::CurrencySymbols);
	SymbolRanges.Add(EUnicodeBlockRange::Latin1Supplement);
	SymbolRanges.Add(EUnicodeBlockRange::LatinExtendedB);
	SymbolRanges.Sort();
	return SymbolRanges;
}

TSharedPtr<SUbCheckBoxList> SUnicodeBrowserWidget::MakeRangeSelector()
{
	auto CheckBoxList = SNew(SUbCheckBoxList)
		.ItemHeaderLabel(FText::FromString(TEXT("Unicode Block Ranges")))
		.IncludeGlobalCheckBoxInHeaderRow(true);
	for (auto const& Range : Ranges)
	{
		int32 const Num = Algo::CountIf(
			Rows,
			[&Range, this](TSharedPtr<FUnicodeBrowserRow> const& Row)
			{
				return Row->BlockRange == Range.Index && (Options->bShowMissing || Row->bCanLoadCodepoint) && (Options->bShowZeroSize || !Row->Measurements.IsZero());
			}
		);
		auto ItemWidget = SNew(SSimpleButton)
			.Text(FText::FromString(FString::Printf(TEXT("%s (%d)"), *Range.DisplayName.ToString(), Num)))
			.ToolTipText(
				FText::FromString(FString::Printf(TEXT("%s: Range %d Codes %d - %d"), *Range.DisplayName.ToString(), Range.Index, Range.Range.GetLowerBoundValue(), Range.Range.GetUpperBoundValue()))
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
		auto const Index = RangeIndices[BlockRange];
		if (!RangeWidgets.IsValidIndex(Index)) return FReply::Unhandled();
		auto const RangeWidget = RangeWidgets[Index];
		RangeScrollbox->ScrollDescendantIntoView(RangeWidget);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply SUnicodeBrowserWidget::OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const
{
	if (CurrentRow == Row) return FReply::Handled();
	CurrentRow = Row;
	CurrentCharacterView->SetText(FText::FromString(CurrentRow->Character));
	CurrentCharacterView->SetToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%04X. Double-Click to copy: %s."), CurrentRow->Codepoint, *CurrentRow->Character)));
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
	auto const NumCols = Options->NumCols;
	auto FilteredRows = Rows.FilterByPredicate(
		[Range, this](TSharedPtr<FUnicodeBrowserRow> const& Row)
		{
			return Row->BlockRange == Range.Index && (Options->bShowMissing || Row->bCanLoadCodepoint) && (Options->bShowZeroSize || !Row->Measurements.IsZero());
		}
	);
	GridPanel->SetMinDesiredSlotHeight(Options->Font.Size * 2);
	GridPanel->SetMinDesiredSlotWidth(Options->Font.Size * 2);
	for (int32 i = 0; i < FilteredRows.Num(); ++i)
	{
		auto const Row = FilteredRows[i];
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
					.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%04X. Double-Click to copy: %s."), Row->Codepoint, *Row->Character)))
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

void SUnicodeCharacterInfo::Construct(FArguments const& InArgs)
{
	Row = InArgs._Row;
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Codepoint: U+%04X (%d)"), Row.Get()->Codepoint, Row.Get()->Codepoint));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Size: X=%-2.0f Y=%-2.0f"), Row.Get()->Measurements.X, Row.Get()->Measurements.Y));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Font: %s"), *Row.Get()->FontData.GetFontFilename()));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Can Load: %s"), *LexToString(Row.Get()->bCanLoadCodepoint)));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Visibility_Lambda(
				[this]()
				{
					return Row.Get()->FontData.GetSubFaceIndex() == 0 ? EVisibility::Collapsed : EVisibility::Visible;
				}
			)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("SubFace Index: %d"), Row.Get()->FontData.GetSubFaceIndex()));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Mono", 9))
			.Visibility_Lambda(
				[this]()
				{
					return Row.Get()->FontData.GetSubFaceIndex() == 0 ? EVisibility::Collapsed : EVisibility::Visible;
				}
			)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Scaling Factor: %3.3f"), Row.Get()->ScalingFactor));
				}
			)
		]
	];
}

TSharedPtr<FUnicodeBrowserRow> SUnicodeCharacterInfo::GetRow() const
{
	return {};
}

void SUnicodeCharacterInfo::SetRow(TSharedPtr<FUnicodeBrowserRow> InRow)
{
	Row = InRow;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

TSharedPtr<SWidget> SUnicodeBrowserWidget::MakeRangeWidget(FUnicodeBlockRange const Range) const
{
	auto const GridPanel = SNew(SUniformGridPanel)
		.SlotPadding(FMargin(6.f, 4.f));
	Options->OnChanged.AddSPLambda(
		GridPanel.ToSharedPtr().Get(),
		[GridPanel, Range, this]()
		{
			RebuildGridColumns(Range, GridPanel);
		}
	);

	RebuildGridColumns(Range, GridPanel);

	return SNew(SExpandableArea)
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

void UUnicodeBrowserOptions::PostInitProperties()
{
	Super::PostInitProperties();
	if (!Font.HasValidFont())
	{
		Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}
}

void UUnicodeBrowserOptions::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnChanged.Broadcast();

	if (!Font.HasValidFont())
	{
		Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}
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
	CurrentRow = Rows.Num() > 0 ? Rows[0] : nullptr;
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
						.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%04X. Double-Click to copy: %s."), CurrentRow->Codepoint, *CurrentRow->Character)))
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
	auto const FontCache = FSlateApplication::Get().GetRenderer()->GetFontCache();

	for (auto const& Range : Ranges)
	{
		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			FString UnicodeString;

			if (FUnicodeChar::CodepointToString(CharCode, UnicodeString))
			{
				float ScalingFactor;
				auto& FontData = FontCache->GetFontDataForCodepoint(FontInfo, CharCode, ScalingFactor);
				auto Row = MakeShared<FUnicodeBrowserRow>();
				Row->Codepoint = CharCode;
				Row->Character = UnicodeString;
				Row->BlockRange = Range.Index;
				Row->FontData = FontData;
				Row->ScalingFactor = ScalingFactor;
				Row->bCanLoadCodepoint = FontCache->CanLoadCodepoint(FontData, CharCode);
				Row->Measurements = FontMeasureService->Measure(UnicodeString, FontInfo);
				Rows.Add(Row);
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
