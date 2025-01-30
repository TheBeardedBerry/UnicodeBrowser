// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FUnicodeBrowserModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void PluginButtonClicked();

private:
	void RegisterMenus();
	TSharedRef<class SDockTab> OnSpawnPluginTab(class FSpawnTabArgs const& SpawnTabArgs);
	TSharedPtr<class FUICommandList> PluginCommands;
};
