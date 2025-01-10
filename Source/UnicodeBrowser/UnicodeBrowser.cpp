// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnicodeBrowser.h"
#include "UnicodeBrowserStyle.h"
#include "UnicodeBrowserCommands.h"
#include "LevelEditor.h"

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "ToolMenus.h"

#include "Engine/Font.h"

#include "Fonts/FontMeasure.h"

#include "Framework/Application/SlateApplication.h"

static const FName UnicodeBrowserTabName("UnicodeBrowser");

#define LOCTEXT_NAMESPACE "FUnicodeBrowserModule"

void FUnicodeBrowserModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FUnicodeBrowserStyle::Initialize();
	FUnicodeBrowserStyle::ReloadTextures();

	FUnicodeBrowserCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FUnicodeBrowserCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FUnicodeBrowserModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUnicodeBrowserModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UnicodeBrowserTabName, FOnSpawnTab::CreateRaw(this, &FUnicodeBrowserModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FUnicodeBrowserTabTitle", "UnicodeBrowser"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FUnicodeBrowserModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FUnicodeBrowserStyle::Shutdown();

	FUnicodeBrowserCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UnicodeBrowserTabName);
}


TSharedRef<SDockTab> FUnicodeBrowserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FUnicodeBrowserModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("UnicodeBrowser.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.FillContentHeight(1.0f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SUnicodeBrowserWidget)
			]
		];
}

void FUnicodeBrowserModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(UnicodeBrowserTabName);
}

void FUnicodeBrowserModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FUnicodeBrowserCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FUnicodeBrowserCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnicodeBrowserModule, UnicodeBrowser)