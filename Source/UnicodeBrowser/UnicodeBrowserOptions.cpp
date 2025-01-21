// SPDX-FileCopyrightText: 2025 NTY.studio
#include "UnicodeBrowserOptions.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"

void UUnicodeBrowserOptions::PostInitProperties()
{
	Super::PostInitProperties();
	if (!FontInfo.HasValidFont())
	{
		FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}

	if(!Font)
	{
		Font = Cast<UFont>(FontInfo.FontObject);
	}
}