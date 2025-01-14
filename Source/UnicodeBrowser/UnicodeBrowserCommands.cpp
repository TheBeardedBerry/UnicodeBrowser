// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnicodeBrowserCommands.h"

#define LOCTEXT_NAMESPACE "FUnicodeBrowserModule"

void FUnicodeBrowserCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Unicode Browser", "Bring up � Unicode Browser window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
