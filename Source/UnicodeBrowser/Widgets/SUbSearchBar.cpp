#include "SUbSearchBar.h"

#include "UnicodeBrowser/UnicodeBrowserOptions.h"

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
				UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange = !UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange;
				UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
				TriggerUpdate();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]() { return UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange; })
		);
			
		SettingsMenu.AddMenuEntry("AutoSetRange", INVTEXT("update ranges"), INVTEXT("should the range selection adjust to the items with matches?\ngenerally this should be enabled,\nunless you want to search for glyphs within a specific range"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
	}

	{
		FUIAction Action(
			FExecuteAction::CreateLambda([this]()
			{
				UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive = !UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive;
				UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
				TriggerUpdate();
			}),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]()	{ return UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive; })
		);
			
		SettingsMenu.AddMenuEntry("CaseSensitive", INVTEXT("case sensitive"), INVTEXT("enabled case sensitive search for letters\nthis setting does only affect single character matching"), FSlateIcon(), Action, EUserInterfaceActionType::ToggleButton);
	}

	return Menu;
}
