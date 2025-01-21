// all rights reserved
#include "SUnicodeBrowserSidePanel.h"

#include "SlateOptMacros.h"
#include "SUnicodeCharacterInfo.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeBrowserSidePanel::Construct(const FArguments& InArgs)
{
	if(!InArgs._UnicodeBrowser.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SUnicodeBrowserSidePanel::Construct] Invalid UnicodeBrowser pointer"));
		return;
	}
	
	UnicodeBrowser = InArgs._UnicodeBrowser;
		
	SSplitter::Construct(SSplitter::FArguments()
	.Orientation(Orient_Vertical)
	.ResizeMode(ESplitterResizeMode::Fill)
		+SSplitter::Slot()
		.SizeRule(SSplitter::FractionOfParent)
		.Value(0.25)
		.MinSize(50)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			[
				SAssignNew(CurrentCharacterView, STextBlock)
				.Visibility(EVisibility::Visible)
				.OnDoubleClicked_Lambda([UnicodeBrowser = UnicodeBrowser](FGeometry const& Geometry, FPointerEvent const& PointerEvent)
				{
					if(!UnicodeBrowser.IsValid())
						return FReply::Unhandled();
					
					FPlatformApplicationMisc::ClipboardCopy(*UnicodeBrowser.Pin().Get()->CurrentRow->Character);					
					return FReply::Handled();
				})
				.Font(UnicodeBrowser.Pin().Get()->CurrentFont)
				.Justification(ETextJustify::Center)
				.IsEnabled(true)
				.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), UnicodeBrowser.Pin().Get()->CurrentRow->Codepoint, *UnicodeBrowser.Pin().Get()->CurrentRow->Character)))
				.Text(FText::FromString(FString::Printf(TEXT("%s"), *UnicodeBrowser.Pin().Get()->CurrentRow->Character)))
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
							.Text(INVTEXT("Character Info"))
						]
						.BodyContent()
						[
							SAssignNew(CurrentCharacterDetails, SUnicodeCharacterInfo).Row(UnicodeBrowser.Pin().Get()->CurrentRow)
						]
					]
					+ SVerticalBox::Slot()
					.FillHeight(1)
					.FillContentHeight(1)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					[
						MakeBlockRangesSidebar()
					]
				]
			]		
	);
	
	UnicodeBrowser.Pin().Get()->OnCharacterHighlight.BindSPLambda(CurrentCharacterView.Get(), [this](FUnicodeBrowserRow *CharacterInfo)
		{
			// update the preview glyph/tooltip
			CurrentCharacterView->SetText(FText::FromString(CharacterInfo->Character));
			CurrentCharacterView->SetToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), CharacterInfo->Codepoint, *CharacterInfo->Character)));

			// update the glyph details
			CurrentCharacterDetails->SetRow(CharacterInfo->AsShared());
		});

	UnicodeBrowser.Pin().Get()->OnFontChanged.AddSPLambda(CurrentCharacterView.Get(), [CurrentCharacterView = CurrentCharacterView](FSlateFontInfo *FontInfo)
		{
			CurrentCharacterView->SetFont(*FontInfo);
		});
}


TSharedRef<SExpandableArea> SUnicodeBrowserSidePanel::MakeBlockRangesSidebar()
{
	RangeSelector = SNew(SUnicodeBlockRangeSelector).UnicodeBrowser(UnicodeBrowser);

	RangeSelector->OnRangeClicked.BindSPLambda(this, [this](EUnicodeBlockRange BlockRange)
	{
		if (UnicodeBrowser.IsValid() && UnicodeBrowser.Pin().Get()->RangeScrollbox.IsValid())
		{
			if (!UnicodeBrowser.Pin().Get()->RangeWidgets.Contains(BlockRange)) return;
			auto const RangeWidget = UnicodeBrowser.Pin().Get()->RangeWidgets.FindChecked(BlockRange);

			// we can't use animated scroll as the layout invalidation of the RangeWidgets would be to early
			// see PR: https://github.com/EpicGames/UnrealEngine/pull/12580
			UnicodeBrowser.Pin().Get()->RangeScrollbox->ScrollDescendantIntoView(RangeWidget, false, EDescendantScrollDestination::TopOrLeft);
			for(auto &[Range, RangeWidget] : UnicodeBrowser.Pin()->RangeWidgets)
			{
				// tell the invalidation boxes that they got moved
				RangeWidget->OnScroll();
			}
		}
	});

	RangeSelector->OnRangeStateChanged.BindSPLambda(this, [this](EUnicodeBlockRange BlockRange, bool State)
	{		
		if (auto const RangeWidget = UnicodeBrowser.Pin().Get()->RangeWidgets.Find(BlockRange)){
			RangeWidget->Get()->SetVisibility(State ? EVisibility::Visible : EVisibility::Collapsed);
		}
	});
	
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
				.OnClicked_Lambda([this](){
					SelectAllRangesWithCharacters();
					return FReply::Handled();
				})
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
				.OnClicked_Lambda([RangeSelector = RangeSelector](){
					RangeSelector->SetRanges(UnicodeBrowser::SymbolRanges);
					return FReply::Handled();
				})
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


void SUnicodeBrowserSidePanel::SelectAllRangesWithCharacters() const
{
	TArray<EUnicodeBlockRange> Ranges;
	for (auto const& [Range, Characters] : UnicodeBrowser.Pin().Get()->Rows)
	{		
		if(!Characters.IsEmpty()) Ranges.Add(Range);
	}
	RangeSelector->SetRanges(Ranges);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
