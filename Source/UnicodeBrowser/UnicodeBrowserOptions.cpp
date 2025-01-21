// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowserOptions.h"

#include "IStructureDetailsView.h"
#include "PropertyEditorModule.h"

#include "Modules/ModuleManager.h"

TSharedRef<IDetailsView> UUnicodeBrowserOptions::MakePropertyEditor(UUnicodeBrowserOptions* Options)
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bShowModifiedPropertiesOption = false;
	DetailsViewArgs.bShowScrollBar = false;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bShowObjectLabel = false;
	DetailsViewArgs.bLockable = false;

	
	auto FontDetailsView = PropertyEditor.CreateDetailView(DetailsViewArgs);
	FontDetailsView->SetObject(Options);
	return FontDetailsView;
}

TSharedRef<SWidget> UUnicodeBrowserOptions::MakePropertyEditorFont(UUnicodeBrowserOptions* Options)
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	TSharedRef<FStructOnScope> StructOnScope = MakeShared<FStructOnScope>(FSlateFontInfo::StaticStruct(), (uint8*)&Options->FontInfo);
	FDetailsViewArgs DetailsViewSettings;
	DetailsViewSettings.bAllowSearch = false;
	DetailsViewSettings.NotifyHook = Options; // send post edit notification to the object
	return PropertyEditor.CreateStructureDetailView(DetailsViewSettings, FStructureDetailsViewArgs(), StructOnScope,FText::GetEmpty())->GetWidget().ToSharedRef();				
}

void UUnicodeBrowserOptions::PostInitProperties()
{
	Super::PostInitProperties();
	if (!FontInfo.HasValidFont())
	{
		FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}
}

void UUnicodeBrowserOptions::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!FontInfo.HasValidFont())
	{
		FontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 18);
	}
	
	OnChanged.Broadcast(&PropertyChangedEvent);
}
