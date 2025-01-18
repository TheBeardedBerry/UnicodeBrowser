// SPDX-FileCopyrightText: 2025 NTY.studio
#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "SlateOptMacros.h"
#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "HAL/PlatformApplicationMisc.h"

#include "Modules/ModuleManager.h"

#include "Widgets/SUnicodeBrowserSidePanel.h"
#include "Widgets/SUnicodeCharacterGridEntry.h"
#include "Widgets/SUnicodeRangeWidget.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeBrowserWidget::Construct(FArguments const& InArgs)
{
	if (!Options)
	{
		Options = NewObject<UUnicodeBrowserOptions>();
		Options->FontInfo = DefaultFont;
	}

	CurrentFont = Options->FontInfo;
	
	Options->OnChanged.RemoveAll(this);
	Options->OnChanged.AddLambda([UnicodeBrowser = AsWeak()](struct FPropertyChangedEvent* PropertyChangedEvent)
	{
		if(UnicodeBrowser.IsValid()){
			static_cast<SUnicodeBrowserWidget*>(UnicodeBrowser.Pin().Get())->MarkDirty();
		}
	});

	bool const bIsInitialized = !Rows.IsEmpty();
	if (!bIsInitialized)
	{
		RangeWidgets.Reset();
		RangeWidgets.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());
		// note character/font data isn't initialized until first tick after construction
		// this permits the browser panel to display earlier
	}
	
	// create a dummy for the preview until the user highlights a character
	if(!CurrentRow.IsValid()){
		CurrentRow = MakeShared<FUnicodeBrowserRow>(UnicodeBrowser::InvalidSubChar, EUnicodeBlockRange::Specials, &CurrentFont);
	}
	
	ChildSlot
	[
		SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			.ResizeMode(ESplitterResizeMode::Fill)
			+ SSplitter::Slot()
			.SizeRule(SSplitter::FractionOfParent)
			.Value(0.7)
			[
				SAssignNew(RangeScrollbox, SScrollBox)
			]
			+ SSplitter::Slot()
			.SizeRule(SSplitter::FractionOfParent)
			.Value(0.3)
			[
				SAssignNew(SidePanel, SUnicodeBrowserSidePanel).UnicodeBrowser(SharedThis(this))
			]				
	];

	if(!bIsInitialized){		
		MarkDirty();
	}
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


void SUnicodeBrowserWidget::Update()
{
	bool const bIsInitialized = !Rows.IsEmpty();

	if(!bIsInitialized || Options->FontInfo.FontObject != CurrentFont.FontObject || Options->FontInfo.TypefaceFontName != CurrentFont.TypefaceFontName){
		CurrentFont = Options->FontInfo;
		
		// rebuild the character list
		PopulateSupportedCharacters();
		
		OnFontChanged.ExecuteIfBound(&CurrentFont);

		if(SidePanel.IsValid() && SidePanel->RangeSelector.IsValid() && RangeScrollbox.IsValid()){
			RangeScrollbox->ClearChildren();
			RangeWidgets.Empty();
			
			// create the range widgets which are represented in this font
			for (auto const& Range : UnicodeBrowser::GetUnicodeBlockRanges())
			{
				// skip ranges which don't have any characters, for now even when showMissing is active!
				if(!Rows.Contains(Range.Index) || Rows.FindChecked(Range.Index).IsEmpty())
					continue;
				
				RangeWidgets.Add(Range.Index,
					SNew(SUnicodeRangeWidget)
						.Range(Range)
						.Visibility(SidePanel->RangeSelector->IsRangeChecked(Range.Index) ? EVisibility::Visible : EVisibility::Collapsed)
						.OnZoomFontSize(this, &SUnicodeBrowserWidget::HandleZoomFont)
						.OnZoomColumnCount(this, &SUnicodeBrowserWidget::HandleZoomColumns)
					);

				RangeScrollbox->AddSlot() [ RangeWidgets.FindChecked(Range.Index).ToSharedRef() ];
			}			
		}
	}
	
	if(!bIsInitialized && SidePanel.IsValid() && SidePanel->RangeSelector.IsValid()){
		// default preset		
		SidePanel->RangeSelector->SetRanges(UnicodeBrowser::SymbolRanges);
	}
	
	RebuildGrid();
}


void SUnicodeBrowserWidget::PopulateSupportedCharacters()
{
	Rows.Empty();
	Rows.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());

	for (auto const& Range : UnicodeBrowser::GetUnicodeBlockRanges())
	{
		Rows.Add(Range.Index);
		TArray<TSharedPtr<FUnicodeBrowserRow>>& RangeArray = Rows.FindChecked(Range.Index);

		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			auto Row = MakeShared<FUnicodeBrowserRow>(CharCode, Range.Index, &CurrentFont);

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


void SUnicodeBrowserWidget::RebuildGrid()
{
	// rebuild the grid
	for (auto & [Range, RangeWidget] : RangeWidgets)
	{
		RebuildGridRange(RangeWidget);
	}
}


void SUnicodeBrowserWidget::RebuildGridRange(TSharedPtr<SUnicodeRangeWidget> RangeWidget)
{
	if(!RangeWidget.IsValid()) return;
	
	TSharedRef<SUniformGridPanel> GridPanel = RangeWidget->GetGridPanel();
	RangeWidget->Characters.Empty();
	GridPanel->ClearChildren();

	if (!Rows.Contains(RangeWidget->GetRange().Index)) return;

	auto const NumCols = Options->NumCols;
	GridPanel->SetMinDesiredSlotHeight(CurrentFont.Size * 2);
	GridPanel->SetMinDesiredSlotWidth(CurrentFont.Size * 2);

	auto RowEntries = Rows.FindChecked(RangeWidget->GetRange().Index);
	// pad the grid with empty slots so that the grid is always filled with a uniform number of columns
	auto const NumEntries = FMath::Max(RowEntries.Num(), NumCols);
	for (int32 i = 0; i < NumEntries; ++i)
	{
		auto Slot = GridPanel->AddSlot(i % NumCols, i / NumCols);
		// empty slot
		if (!RowEntries.IsValidIndex(i)) continue;

		auto const Row = RowEntries[i];
		TSharedPtr<SUnicodeCharacterGridEntry> GridCell = SNew(SUnicodeCharacterGridEntry)
				.FontInfo(CurrentFont)
				.UnicodeCharacter(Row)
				.OnMouseDoubleClick_Lambda([Character = Row->Character](FGeometry const& Geometry, FPointerEvent const& PointerEvent)
				{
					FPlatformApplicationMisc::ClipboardCopy(*Character);
					return FReply::Handled();
				})
				.OnMouseMove(this, &SUnicodeBrowserWidget::OnCharacterMouseMove, Row);

		RangeWidget->Characters.Add(GridCell);
		
		Slot
		[
			GridCell.ToSharedRef()
		];
	}
}


FReply SUnicodeBrowserWidget::OnCharacterMouseMove(FGeometry const& Geometry, FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const
{
	if (CurrentRow == Row) return FReply::Unhandled();
	CurrentRow = Row;

	OnCharacterHighlight.ExecuteIfBound(Row.Get());	
	return FReply::Handled();
}


void SUnicodeBrowserWidget::HandleZoomFont(float Offset)
{	
	Options->FontInfo.Size = FMath::Max(1.0f, Options->FontInfo.Size + Offset);

	// update each entry with the new fontsize
	for(auto &[Range, RangeWidget] : RangeWidgets)
	{
		for(auto &CharacterGridEntry : RangeWidget->Characters)
		{
			CharacterGridEntry->SetFontInfo(Options->FontInfo);			
		}
	}
}


void SUnicodeBrowserWidget::HandleZoomColumns(float Offset)
{
	// we want inverted behavior for columns
	Options->NumCols = FMath::Max(1, FMath::RoundToInt(Options->NumCols - Offset));
	
	RebuildGrid();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
