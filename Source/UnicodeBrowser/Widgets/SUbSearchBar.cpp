#include "SUbSearchBar.h"

void SUbSearchBar::Construct(const FArguments& InArgs)
{
	OnTextChanged = InArgs._OnTextChanged;
	
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
					SAssignNew(TextBox, SEditableTextBox)
					.HintText(INVTEXT("search"))
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
				.Size(FVector2D(20, 1))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CheckBox_AutoSetRanges, SCheckBox)
				.IsChecked(true)
				[
					SNew(STextBlock)
					.Text(INVTEXT("update ranges"))
				]
			]
		]
	);
}
