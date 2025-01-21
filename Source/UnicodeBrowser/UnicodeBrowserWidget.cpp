// SPDX-FileCopyrightText: 2025 NTY.studio
#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "ISinglePropertyView.h"
#include "SlateOptMacros.h"
#include "Fonts/UnicodeBlockRange.h"

#include "Framework/Application/SlateApplication.h"

#include "Modules/ModuleManager.h"

#include "Widgets/SUnicodeBrowserSidePanel.h"
#include "Widgets/SUnicodeCharacterGridEntry.h"
#include "Widgets/SUnicodeRangeWidget.h"
#include "Widgets/Images/SLayeredImage.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SUnicodeBrowserWidget::~SUnicodeBrowserWidget()
{
	UToolMenus::Get()->RemoveMenu("UnicodeBrowser.Settings");
	UToolMenus::Get()->RemoveMenu("UnicodeBrowser.Font");
}

void SUnicodeBrowserWidget::Construct(FArguments const& InArgs)
{
	if(bInitialized)
		return;

	bInitialized = true;
	
	CurrentFont = UUnicodeBrowserOptions::Get()->GetFontInfo();
	
	UUnicodeBrowserOptions::Get()->OnFontChanged.RemoveAll(this);
	UUnicodeBrowserOptions::Get()->OnFontChanged.AddLambda([UnicodeBrowser = AsWeak()]()
	{
		if(UnicodeBrowser.IsValid()){
			static_cast<SUnicodeBrowserWidget*>(UnicodeBrowser.Pin().Get())->MarkDirty();
		}
	});

	RangeWidgets.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());
	
	// create a dummy for the preview until the user highlights a character
	CurrentRow = MakeShared<FUnicodeBrowserRow>(UnicodeBrowser::InvalidSubChar, EUnicodeBlockRange::Specials, &CurrentFont);
	

	SearchBar = SNew(SUbSearchBar)
		.OnTextChanged(this, &SUnicodeBrowserWidget::FilterByString);
	
	
	// generate the settings context menu
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("UnicodeBrowser.Settings");	
	UToolMenus::Get()->AssembleMenuHierarchy(Menu,
	{
		this->CreateMenuSection_Settings(),
		SearchBar->CreateMenuSection_Settings()
	});

	TSharedPtr<SLayeredImage> FilterImage = SNew(SLayeredImage)
			.Image(FAppStyle::Get().GetBrush("DetailsView.ViewOptions"))
			.ColorAndOpacity(FSlateColor::UseForeground());

	TSharedPtr<SComboButton> SettingsButton = SNew( SComboButton )
		.HasDownArrow(false)
		.ContentPadding(0.0f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.ButtonStyle( FAppStyle::Get(), "SimpleButton" )
		.MenuContent()
		[			
			UToolMenus::Get()->GenerateWidget(Menu->GetMenuName(), Menu)			
		]
		.ButtonContent()
		[
			FilterImage.ToSharedRef()
		];


	// generate the settings context menu
	UToolMenu* MenuFont = UToolMenus::Get()->RegisterMenu("UnicodeBrowser.Font");

	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	
	FToolMenuSection &PresetSettingsSection = MenuFont->AddSection(TEXT("PresetSettings"), INVTEXT("preset"));
	{
		FSinglePropertyParams SinglePropertyParams;
		SinglePropertyParams.NamePlacement = EPropertyNamePlacement::Type::Hidden;
		TSharedRef<SWidget> Widget = SNew(SBox)
			.Padding(15, 0, 0, 0)
			[
				PropertyEditor.CreateSingleProperty(UUnicodeBrowserOptions::Get(), "Preset", SinglePropertyParams).ToSharedRef()
			];
				
		PresetSettingsSection.AddEntry(FToolMenuEntry::InitWidget("FontInfo",  Widget, FText::GetEmpty()));
	}
	
	FToolMenuSection &FontSettingsSection = MenuFont->AddSection(TEXT("FontSettings"), INVTEXT("font"));
	{		
		FSinglePropertyParams SinglePropertyParams;
		SinglePropertyParams.NamePlacement = EPropertyNamePlacement::Type::Hidden;
		TSharedRef<SWidget> Widget = SNew(SBox)
			.Padding(15, 0, 0, 0)
			[
				PropertyEditor.CreateSingleProperty(UUnicodeBrowserOptions::Get(), "Font", SinglePropertyParams).ToSharedRef()
			];
		FontSettingsSection.AddEntry(FToolMenuEntry::InitWidget("FontInfo",  Widget, FText::GetEmpty()));
	}

	{		
		FSinglePropertyParams SinglePropertyParams;
		SinglePropertyParams.NameOverride = INVTEXT("Typeface");
		TSharedRef<SWidget> Widget = SNew(SBox)
			.Padding(15, 0, 0, 0)
			[
				PropertyEditor.CreateSingleProperty(UUnicodeBrowserOptions::Get(), "FontTypeFace", SinglePropertyParams).ToSharedRef()
			];
		FontSettingsSection.AddEntry(FToolMenuEntry::InitWidget("FontTypeFace",  Widget, FText::GetEmpty()));
	}
	
	{
		TSharedRef<SWidget> Widget = SNew(SBox)
		.Padding(15, 0, 0, 0)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(INVTEXT("font size"))
			]
			+SHorizontalBox::Slot()
			.Padding(10, 0, 10, 0)
			.VAlign(VAlign_Center)
			[
				SNew(SSpinBox<float>)
				.Delta(1)
				.Justification(EHorizontalAlignment::HAlign_Left)
				.MinValue(6)
				.MinDesiredWidth(150)
				.MaxValue(200)
				.Value_Lambda([this](){ return CurrentFont.Size; })
				.OnValueChanged_Lambda([this](float CurrentValue)
				{
					CurrentFont.Size = CurrentValue;
					UUnicodeBrowserOptions::Get()->SetFontInfo(CurrentFont);
					RebuildGrid();
				})
			]
		];
			
		FontSettingsSection.AddEntry(FToolMenuEntry::InitWidget("BrowserFontSize",  Widget, FText::GetEmpty(), false, false, false, INVTEXT("you can use CTRL + MouseWheel to adjust font size in the browser")));
	}
	
	TSharedPtr<SComboButton> SettingsButtonFont = SNew( SComboButton )
		.HasDownArrow(true)
		.ContentPadding(0.0f)
		.ForegroundColor( FSlateColor::UseForeground() )
		.ButtonStyle( FAppStyle::Get(), "SimpleButton" )
		.MenuContent()
		[			
			UToolMenus::Get()->GenerateWidget(MenuFont->GetMenuName(), MenuFont)			
		]
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(INVTEXT("Preset / Font"))
		];
	
	
	ChildSlot
	[
		SNew(SSplitter)
		.Orientation(Orient_Vertical)
		.ResizeMode(ESplitterResizeMode::Fill)
		+ SSplitter::Slot()
			.SizeRule(SSplitter::SizeToContent)
			[
				SNew(SBox)
				.Padding(10)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SearchBar.ToSharedRef()
					]
					+SHorizontalBox::Slot()
					.AutoWidth()			
					[
						SNew(SSpacer)
						.Size(FVector2D(10, 1))
					]					
					+SHorizontalBox::Slot()
					.AutoWidth()			
					[
						SNew(SSpacer)
						.Size(FVector2D(10, 1))
					]					
					+SHorizontalBox::Slot()
					.AutoWidth()			
					[
						SettingsButtonFont.ToSharedRef()
					]
					+SHorizontalBox::Slot()
					.AutoWidth()			
					[
						SNew(SSpacer)
						.Size(FVector2D(10, 1))
					]					
					+SHorizontalBox::Slot()
					.AutoWidth()			
					[
						SettingsButton.ToSharedRef()
					]
				]
			]
			+ SSplitter::Slot()
			.SizeRule(SSplitter::FractionOfParent)
			.Value(1.0)
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
			]
	];

	SetSidePanelVisibility(UUnicodeBrowserOptions::Get()->bShowSidePanel);
	RangeScrollbox->SetScrollBarRightClickDragAllowed(true);

	MarkDirty();
}

UToolMenu* SUnicodeBrowserWidget::CreateMenuSection_Settings()
{
	UToolMenu *Menu = UToolMenus::Get()->GenerateMenu("UnicodeBrowser.Settings.General_Display", FToolMenuContext());
	
	FToolMenuSection &GeneralSettingsSection = Menu->AddSection(TEXT("GeneralSettings"), INVTEXT("general settings"));
	{
		{
			FUIAction Action = FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					UUnicodeBrowserOptions::Get()->bShowSidePanel = !UUnicodeBrowserOptions::Get()->bShowSidePanel;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					SetSidePanelVisibility(UUnicodeBrowserOptions::Get()->bShowSidePanel);
					RebuildGrid();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return UUnicodeBrowserOptions::Get()->bShowSidePanel; })
			);
			
			GeneralSettingsSection.AddMenuEntry("ShowSidePanel", INVTEXT("show sidepanel"), FText::GetEmpty(), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
		}
		
		{
			FUIAction Action = FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					UUnicodeBrowserOptions::Get()->bShowMissing = !UUnicodeBrowserOptions::Get()->bShowMissing;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					UpdateCharacters();
					RebuildGrid();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]() { return UUnicodeBrowserOptions::Get()->bShowMissing; })
			);
			
			GeneralSettingsSection.AddMenuEntry("ShowMissingCharacters", INVTEXT("show missing characters"), INVTEXT("should characters which are missing in the font be shown?"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
		}

		{
			FUIAction Action = FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					UUnicodeBrowserOptions::Get()->bShowZeroSize = !UUnicodeBrowserOptions::Get()->bShowZeroSize;	
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					UpdateCharacters();
					RebuildGrid();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]()	{ return UUnicodeBrowserOptions::Get()->bShowZeroSize; })
			);
			
			GeneralSettingsSection.AddMenuEntry("ShowZeroSizeCharacters", INVTEXT("show zero size"), INVTEXT("show Characters which have a measurement of 0x0 (primary for debug purposes)"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
		}

		{
			FUIAction Action = FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					UUnicodeBrowserOptions::Get()->bCacheCharacterMetaOnLoad = !UUnicodeBrowserOptions::Get()->bCacheCharacterMetaOnLoad;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]()	{ return UUnicodeBrowserOptions::Get()->bCacheCharacterMetaOnLoad; })
			);
			
			GeneralSettingsSection.AddMenuEntry("CacheCharacterMeta", INVTEXT("cache character meta data"), INVTEXT("Cache the Character meta information while loading the font, this is slower while changing fonts, but may reduce delay for displaying character previews"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
		}

		{
			FUIAction Action = FUIAction(
				FExecuteAction::CreateLambda([this]()
				{
					UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange = !UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					if(UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange)
					{
						SidePanel->SelectAllRangesWithCharacters();
					}
				}),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([this]()	{ return UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange; })
			);
			
			GeneralSettingsSection.AddMenuEntry("AutoSetRangeOnFontChange", INVTEXT("auto set ranges on font change"), INVTEXT("automatic enable all ranges which are covered by the current font"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
		}
	}

	FToolMenuSection &DisplaySettingsSection = Menu->AddSection(TEXT("DisplaySettings"), INVTEXT("display settings"));
	{
		{
			TSharedRef<SWidget> Widget = SNew(SBox)
			.Padding(15, 0, 0, 0)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(INVTEXT("columns"))
				]
				+SHorizontalBox::Slot()
				.Padding(10, 0, 10, 0)
				.VAlign(VAlign_Center)
				[
					SNew(SSpinBox<int32>)
					.Delta(1)
					.MinValue(1)
					.MaxValue(32)
					.Value_Lambda([this]()  { return UUnicodeBrowserOptions::Get()->NumCols; })
					.OnValueChanged_Lambda([this](int32 CurrentValue)
					{
						UUnicodeBrowserOptions::Get()->NumCols = CurrentValue;						
						UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
						RebuildGrid();
					})
				]
			];
			
			DisplaySettingsSection.AddEntry(FToolMenuEntry::InitWidget("BrowserNumColumns",  Widget, FText::GetEmpty(), false, false, false, INVTEXT("you can use CTRL + Shift + MouseWheel to adjust column count in the browser")));
		}
		
		
	}

	return Menu;
}

void SUnicodeBrowserWidget::SetSidePanelVisibility(bool bVisible)
{
	SidePanel->SetVisibility(bVisible ? EVisibility::Visible : EVisibility::Collapsed);
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


void SUnicodeBrowserWidget::Update(bool bForceRepopulateCharacters)
{
	bool const bIsInitialized = !Rows.IsEmpty();

	// did the actual font change?
	bool const bUpdateCharacters = !bIsInitialized || bForceRepopulateCharacters || UUnicodeBrowserOptions::Get()->GetFontInfo().FontObject != CurrentFont.FontObject || UUnicodeBrowserOptions::Get()->GetFontInfo().TypefaceFontName != CurrentFont.TypefaceFontName;
	// did only style parameters of the FontInfo change?!
	bool const bUpdateCharacterPreviews = bUpdateCharacters || UUnicodeBrowserOptions::Get()->GetFontInfo() != CurrentFont; 
	CurrentFont = UUnicodeBrowserOptions::Get()->GetFontInfo();
	
	
	if(bUpdateCharacters)
	{
		// rebuild the character list
		PopulateSupportedCharacters();

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

				RangeScrollbox->AddSlot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Top)
					[ RangeWidgets.FindChecked(Range.Index).ToSharedRef() ];
			}			
		}

		if(UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange)
		{
			SidePanel->SelectAllRangesWithCharacters();
		}
		
		OnFontChanged.Broadcast(&CurrentFont);
	}
	
	if(bUpdateCharacterPreviews){
		// update each entry with the new font info
		for(auto &[Range, RangeWidget] : RangeWidgets)
		{
			for(auto &CharacterGridEntry : RangeWidget->Characters)
			{
				CharacterGridEntry->SetFontInfo(CurrentFont);			
			}
		}
	}

	
	if(!bIsInitialized){
		if(SidePanel.IsValid() && SidePanel->RangeSelector.IsValid())
		{
			// only use a default preset if this option is disabled, otherwise it causes trouble when reopening the Tab while a valid font is still set in UUnicodeBrowserOptions 
			if(!UUnicodeBrowserOptions::Get()->bAutoSetRangeOnFontChange)
			{
				// default preset		
				SidePanel->RangeSelector->SetRanges(UnicodeBrowser::SymbolRanges);
			}	
		}
	}
	
	RebuildGrid();
}


void SUnicodeBrowserWidget::PopulateSupportedCharacters()
{
	RowsRaw.Empty();
	RowsRaw.Reserve(UnicodeBrowser::GetUnicodeBlockRanges().Num());

	for (auto const& Range : UnicodeBrowser::GetUnicodeBlockRanges())
	{
		RowsRaw.Add(Range.Index);
		TArray<TSharedPtr<FUnicodeBrowserRow>>& RangeArray = RowsRaw.FindChecked(Range.Index);

		for (int CharCode = Range.Range.GetLowerBound().GetValue(); CharCode <= Range.Range.GetUpperBound().GetValue(); ++CharCode)
		{
			auto Row = MakeShared<FUnicodeBrowserRow>(CharCode, Range.Index, &CurrentFont);

			if (UUnicodeBrowserOptions::Get()->bCacheCharacterMetaOnLoad)
			{
				Row->Preload();
			}

			RangeArray.Add(Row);
		}
	}

	UpdateCharacters();
}

void SUnicodeBrowserWidget::UpdateCharacters()
{
	Rows.Empty();
	Rows.Reserve(RowsRaw.Num());
	
	for(const auto &[Range, RawRangeRows] : RowsRaw)
	{
		TArray<TSharedPtr<FUnicodeBrowserRow>> RowsFiltered = RawRangeRows.FilterByPredicate([](TSharedPtr<FUnicodeBrowserRow> const &RawRow)
		{
			if(RawRow->bFilteredByTag)
				return false;
			
			if (!UUnicodeBrowserOptions::Get()->bShowMissing && !RawRow->CanLoadCodepoint())
				return false;

			if(!UUnicodeBrowserOptions::Get()->bShowZeroSize && RawRow->GetMeasurements().IsZero())
				return false;

			return true;
		});

		if(!RowsFiltered.IsEmpty())
		{
			Rows.Add(Range, RowsFiltered);
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

void SUnicodeBrowserWidget::InvalidateGrid()
{
	for (auto & [Range, RangeWidget] : RangeWidgets)
	{
		RangeWidget->Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void SUnicodeBrowserWidget::FilterByString(FString Needle)
{
	bool const bFilterTags = Needle.Len() > 0 && UUnicodeBrowserOptions::Get()->Preset && UUnicodeBrowserOptions::Get()->Preset->SupportsFont(CurrentFont);

	// build an array which include all single character search terms
	TArray<FString> CharacterNeedles;
	Needle.ParseIntoArray(CharacterNeedles, TEXT(","));
	for(int Idx=CharacterNeedles.Num() - 1; Idx >= 0; Idx--)
	{
		CharacterNeedles[Idx].TrimStartAndEndInline();
		if(CharacterNeedles[Idx].Len() > 1)
		{
			CharacterNeedles.RemoveAtSwap(Idx);
		}
	}
	
	bool const bFilterByCharacter = CharacterNeedles.Num() > 0;

	bool const bCaseSensitive = UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive;
	
	bool bNeedUpdate = false;
	
	TArray<int32> Whitelist;
	if(bFilterTags){
		Whitelist = UUnicodeBrowserOptions::Get()->Preset->GetCharactersByNeedle(Needle);
	}
	
	for(auto &[Range, RawRangeRows] : RowsRaw)
	{
		for(TSharedPtr<FUnicodeBrowserRow> &RowRaw : RawRangeRows)
		{
			bool bToggleRowState = Needle.IsEmpty() && RowRaw->bFilteredByTag;
			
			if(!bToggleRowState && bFilterTags)
			{
				bToggleRowState |= !Whitelist.Contains(RowRaw->Codepoint) != RowRaw->bFilteredByTag;
			}

			if(!bToggleRowState && bFilterByCharacter)
			{
				bool const bMatchesCharacter = CharacterNeedles.ContainsByPredicate([bCaseSensitive, RowRaw](FString const &CharacterNeedle)
				{
					return CharacterNeedle.Equals(RowRaw->Character, bCaseSensitive ? ESearchCase::CaseSensitive : ESearchCase::IgnoreCase);
				});
				
				bToggleRowState |= !bMatchesCharacter != RowRaw->bFilteredByTag;				
			}

			if(bToggleRowState)
			{
				RowRaw->bFilteredByTag = !RowRaw->bFilteredByTag;
				bNeedUpdate = true;
			}
		}
	}

	if(bNeedUpdate){
		UpdateCharacters();
		RebuildGrid();

		if(UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange)
		{
			SidePanel->SelectAllRangesWithCharacters();
		}
	}
}


void SUnicodeBrowserWidget::RebuildGridRange(TSharedPtr<SUnicodeRangeWidget> RangeWidget)
{
	if(!RangeWidget.IsValid()) return;
	
	TSharedRef<SUniformGridPanel> GridPanel = RangeWidget->GetGridPanel();
	RangeWidget->Characters.Empty();
	GridPanel->ClearChildren();

	if (!Rows.Contains(RangeWidget->GetRange().Index)) return;

	auto const NumCols = UUnicodeBrowserOptions::Get()->NumCols;
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
				.OnMouseMove(this, &SUnicodeBrowserWidget::OnCharacterMouseMove, Row);

		RangeWidget->Characters.Add(GridCell);
		
		Slot
		[
			GridCell.ToSharedRef()
		];
	}

	RangeWidget->Invalidate(EInvalidateWidgetReason::Layout);
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
	CurrentFont.Size = FMath::Max(1.0f, CurrentFont.Size + Offset);
	UUnicodeBrowserOptions::Get()->SetFontInfo(CurrentFont);
	InvalidateGrid();
}


void SUnicodeBrowserWidget::HandleZoomColumns(float Offset)
{
	// we want inverted behavior for columns
	UUnicodeBrowserOptions::Get()->NumCols = FMath::Max(1, FMath::RoundToInt(UUnicodeBrowserOptions::Get()->NumCols - Offset));
	
	RebuildGrid();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
