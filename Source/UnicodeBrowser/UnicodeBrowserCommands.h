// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UnicodeBrowserStyle.h"

#include "Framework/Commands/Commands.h"

class FUnicodeBrowserCommands : public TCommands<FUnicodeBrowserCommands>
{
public:
	FUnicodeBrowserCommands()
		: TCommands<FUnicodeBrowserCommands>(TEXT("UnicodeBrowser"), NSLOCTEXT("Contexts", "UnicodeBrowser", "Unicode Browser"), NAME_None, FUnicodeBrowserStyle::GetStyleSetName()) {}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
