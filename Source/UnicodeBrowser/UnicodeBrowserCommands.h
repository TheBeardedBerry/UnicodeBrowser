// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "UnicodeBrowserStyle.h"

class FUnicodeBrowserCommands : public TCommands<FUnicodeBrowserCommands>
{
public:
	FUnicodeBrowserCommands()
		: TCommands<FUnicodeBrowserCommands>(TEXT("UnicodeBrowser"), NSLOCTEXT("Contexts", "UnicodeBrowser", "UnicodeBrowser Plugin"), NAME_None, FUnicodeBrowserStyle::GetStyleSetName()) {}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
