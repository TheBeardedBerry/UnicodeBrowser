#include "SUbSearchBar.h"

#include "Widgets/Images/SLayeredImage.h"

void SUbSearchBar::Construct(const FArguments& InArgs)
{
	OnTextChanged = InArgs._OnTextChanged;

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
			SNew(SBox)
			.Padding(5)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(CheckBox_AutoSetRanges, SCheckBox)
					.IsChecked(true)
					.Padding(FMargin(5, 0, 0, 0))
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State) { TriggerUpdate(); })
					[
						SNew(STextBlock)
						.Text(INVTEXT("update ranges"))
					]
				]			
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SSpacer)
					.Size(FVector2D(1, 5))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(CheckBox_CaseSensitive, SCheckBox)
					.IsChecked(false)
					.Padding(FMargin(5, 0, 0, 0))
					.OnCheckStateChanged_Lambda([this](ECheckBoxState State) { TriggerUpdate(); })
					.ToolTipText(INVTEXT("enabled case sensitive search for letters\nthis setting doesn't affect Tag search"))
					[
						SNew(STextBlock)
						.Text(INVTEXT("case sensitive"))
					]				
				]
			]
		]
		.ButtonContent()
		[
			FilterImage.ToSharedRef()
		];

	
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
