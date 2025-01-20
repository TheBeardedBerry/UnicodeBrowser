#include "SUbSearchBar.h"

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

void SUbSearchBar::Construct(const FArguments& InArgs)
{
	OnTextChanged = InArgs._OnTextChanged;

	SSearchBox::Construct(SSearchBox::FArguments()
		.MinDesiredWidth(300)
		.DelayChangeNotificationsWhileTyping(true)
		.OnTextChanged_Lambda([this](const FText& Text)
		{
			OnTextChanged.ExecuteIfBound(Text.ToString());
		})
	);
}

void SUbSearchBar::CreateMenuSection_Settings(UToolMenu *Menu)
{
	FToolMenuSection &SettingsMenu = Menu->AddSection(TEXT("SearchSettings"), INVTEXT("search settings"));	

	// widget based example, keep this in for now
	/*TSharedRef<SCheckBox> AutoSetRangeWidget = SNew(SCheckBox)
				 .IsChecked(true)
				 .Padding(FMargin(5, 0, 0, 0))
				 .OnCheckStateChanged_Lambda([this](ECheckBoxState State) { bAutoSetRange = State == ECheckBoxState::Checked; TriggerUpdate(); })
				 [
					 SNew(STextBlock)
					 .Text(INVTEXT("update ranges"))
				 ];
	SettingsMenu.AddEntry(FToolMenuEntry::InitWidget("AutoSetRange",  AutoSetRangeWidget, FText::GetEmpty()));

	*/
	{
		FUIAction Action(
			FExecuteAction::CreateLambda([this]()
			{
				bAutoSetRange = !bAutoSetRange;
				TriggerUpdate();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]() { return bAutoSetRange; })
		);
			
		SettingsMenu.AddMenuEntry("AutoSetRange", INVTEXT("update ranges"), INVTEXT(""), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
	}

	{
		FUIAction Action(
			FExecuteAction::CreateLambda([this]()
			{
				bCaseSensitive = !bCaseSensitive;
				TriggerUpdate();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]()	{ return bCaseSensitive; })
		);
			
		SettingsMenu.AddMenuEntry("CaseSensitive", INVTEXT("case sensitive"), INVTEXT("enabled case sensitive search for letters\nthis setting doesn't affect Tag search"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
	}

}
