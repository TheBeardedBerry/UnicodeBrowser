// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "Styling/SlateStyle.h"

class FUnicodeBrowserStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static ISlateStyle const& Get();
	static FName GetStyleSetName();

private:
	static TSharedRef<class FSlateStyleSet> Create();

private:
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
