#include "SUbSearchBar.h"

#include "SSimpleButton.h"

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
				.Size(FVector2D(5, 1))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()			
			[
				SNew(SSimpleButton)
				.Text(INVTEXT("X"))
				.ToolTipText(INVTEXT("reset search"))
				.OnClicked_Lambda([this]()
				{
					TextBox->SetText(FText::GetEmpty());
					return FReply::Handled();
				})
			]
			+SHorizontalBox::Slot()
			.AutoWidth()			
			[
				SNew(SSpacer)
				.Size(FVector2D(50, 1))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CheckBox_AutoSetRanges, SCheckBox)
				.IsChecked(true)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState State) { TriggerUpdate(); })
				[
					SNew(STextBlock)
					.Text(INVTEXT("update ranges"))
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
				SAssignNew(CheckBox_CaseSensitive, SCheckBox)
				.IsChecked(true)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState State) { TriggerUpdate(); })
				.ToolTipText(INVTEXT("enabled case sensitive search for letters\nthis setting doesn't affect Tag search"))
				[
					SNew(STextBlock)
					.Text(INVTEXT("case sensitive"))
				]
				
			]
		]
	);
}
