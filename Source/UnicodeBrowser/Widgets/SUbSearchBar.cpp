#include "SUbSearchBar.h"

#include "Widgets/Images/SLayeredImage.h"

void SUbSearchBar::Construct(const FArguments& InArgs)
{
	OnTextChanged = InArgs._OnTextChanged;

	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("UnicodeBrowser.Search.SettingsMenu");

	// create menu (scoped to keep it organized)
	{
		FToolMenuSection &SettingsMenu = Menu->AddSection(TEXT("SearchSettings"), INVTEXT("search settings"));

		TSharedRef<SCheckBox> AutoSetRangeWidget = SNew(SCheckBox)
					 .IsChecked(true)
					 .Padding(FMargin(5, 0, 0, 0))
					 .OnCheckStateChanged_Lambda([this](ECheckBoxState State) { bAutoSetRange = State == ECheckBoxState::Checked; TriggerUpdate(); })
					 [
						 SNew(STextBlock)
						 .Text(INVTEXT("update ranges"))
					 ];
		SettingsMenu.AddEntry(FToolMenuEntry::InitWidget("AutoSetRange",  AutoSetRangeWidget, FText::GetEmpty()));
		
		TSharedRef<SCheckBox> CaseSensitiveWidget = SNew(SCheckBox)
				   .IsChecked(false)
				   .Padding(FMargin(5, 0, 0, 0))
				   .OnCheckStateChanged_Lambda([this](ECheckBoxState State) { bCaseSensitive = State == ECheckBoxState::Checked; TriggerUpdate(); })
				   .ToolTipText(INVTEXT("enabled case sensitive search for letters\nthis setting doesn't affect Tag search"))
				   [
					   SNew(STextBlock)
					   .Text(INVTEXT("case sensitive"))
				   ];

		SettingsMenu.AddEntry(FToolMenuEntry::InitWidget("CaseSensitive",  CaseSensitiveWidget, FText::GetEmpty()));

		TSharedPtr<SLayeredImage> FilterImage = SNew(SLayeredImage)
			.Image(FAppStyle::Get().GetBrush("DetailsView.ViewOptions"))
			.ColorAndOpacity(FSlateColor::UseForeground());

		TSharedPtr<SComboButton> SettingsButton = SNew( SComboButton )
			.HasDownArrow(false)
			.ContentPadding(0.0f)
			.ForegroundColor( FSlateColor::UseForeground() )
			.ButtonStyle( FAppStyle::Get(), "SimpleButton" )
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
			.MenuContent()
			[			
				UToolMenus::Get()->GenerateWidget(Menu->GetMenuName(), Menu)			
			]
			.ButtonContent()
			[
				FilterImage.ToSharedRef()
			];
	}
	
	SBox::Construct(SBox::FArguments()
		.Padding(10)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(SBox)
				.MinDesiredWidth(300)
				.HAlign(HAlign_Fill)
				[
					SAssignNew(SearchBox, SSearchBox)
					.DelayChangeNotificationsWhileTyping(true)
					.OnTextChanged_Lambda([this](const FText& Text)
					{
						OnTextChanged.ExecuteIfBound(Text.ToString());
					})
				]
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
	);
}
