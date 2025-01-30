// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnicodeBrowserStyle.h"

#include "Framework/Application/SlateApplication.h"

#include "Interfaces/IPluginManager.h"

#include "Slate/SlateGameResources.h"

#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FUnicodeBrowserStyle::StyleInstance = nullptr;

void FUnicodeBrowserStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FUnicodeBrowserStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FUnicodeBrowserStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("UnicodeBrowserStyle"));
	return StyleSetName;
}

FVector2D const Icon16x16(16.0f, 16.0f);
FVector2D const Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FUnicodeBrowserStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("UnicodeBrowserStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("UnicodeBrowser")->GetBaseDir() / TEXT("Resources"));
	
	Style->Set("UnicodeBrowser.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FUnicodeBrowserStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

ISlateStyle const& FUnicodeBrowserStyle::Get()
{
	return *StyleInstance;
}
