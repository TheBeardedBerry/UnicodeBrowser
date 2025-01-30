#include "SUbSearchBar.h"

#include "ToolMenus.h"

#include "UnicodeBrowser/UnicodeBrowserOptions.h"

void SUbSearchBar::Construct(FArguments const& InArgs)
{
	OnTextChanged = InArgs._OnTextChanged;

	SSearchBox::Construct(
		SSearchBox::FArguments()
		.MinDesiredWidth(300)
		.DelayChangeNotificationsWhileTyping(true)
		.OnTextChanged_Lambda(
			[this](FText const& Text)
			{
				OnTextChanged.ExecuteIfBound(Text.ToString());
			}
		)
	);
}

UToolMenu* SUbSearchBar::CreateMenuSection_Settings() const
{
	UToolMenu* Menu = UToolMenus::Get()->GenerateMenu("UnicodeBrowser.SearchSettings", FToolMenuContext());

	FToolMenuSection& SettingsMenu = Menu->AddSection(TEXT("SearchSettings"), INVTEXT("Search Settings"));

	{
		FUIAction const Action(
			FExecuteAction::CreateLambda(
				[this]()
				{
					UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange = !UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					TriggerUpdate();
				}
			),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]() { return UUnicodeBrowserOptions::Get()->bSearch_AutoSetRange; })
		);

		SettingsMenu.AddMenuEntry(
			"AutoSetRange",
			INVTEXT("Update Ranges"),
			INVTEXT("Should the range selection adjust to the items with matches?\nGenerally this should be enabled,\nunless you want to search for glyphs within a specific range"),
			FSlateIcon(),
			Action,
			EUserInterfaceActionType::ToggleButton
		);
	}

	{
		FUIAction const Action(
			FExecuteAction::CreateLambda(
				[this]()
				{
					UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive = !UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive;
					UUnicodeBrowserOptions::Get()->TryUpdateDefaultConfigFile();
					TriggerUpdate();
				}
			),
			FCanExecuteAction(),
			FIsActionChecked::CreateLambda([this]() { return UUnicodeBrowserOptions::Get()->bSearch_CaseSensitive; })
		);

		SettingsMenu.AddMenuEntry(
			"CaseSensitive",
			INVTEXT("Case Sensitive"),
			INVTEXT("Enabled case sensitive search for letters\nThis setting does only affect single character matching"),
			FSlateIcon(),
			Action,
			EUserInterfaceActionType::ToggleButton
		);
	}

	return Menu;
}
