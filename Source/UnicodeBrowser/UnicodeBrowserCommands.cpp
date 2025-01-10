// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnicodeBrowserCommands.h"

#define LOCTEXT_NAMESPACE "FUnicodeBrowserModule"

void FUnicodeBrowserCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "UnicodeBrowser", "Bring up UnicodeBrowser window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
