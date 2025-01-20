#include "SUbSearchBar.h"

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

UToolMenu* SUbSearchBar::CreateMenuSection_Settings()
{
	UToolMenu *Menu = UToolMenus::Get()->GenerateMenu("UnicodeBrowser.SearchSettings", FToolMenuContext());
	
	FToolMenuSection &SettingsMenu = Menu->AddSection(TEXT("SearchSettings"), INVTEXT("search settings"));	

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

	return Menu;
}
