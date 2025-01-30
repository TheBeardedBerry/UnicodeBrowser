// all rights reserved
#include "GlyphTagsImportFactory.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UnicodeBrowser/DataAsset_FontTags.h"

UGlyphTagsImportFactory::UGlyphTagsImportFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Factory
	SupportedClass = UDataAsset_FontTags::StaticClass();
	Formats.Add("json;Unicode Glyph Tags");
	bCreateNew = false;
	//bEditAfterNew = true;	
	bEditorImport = true;
	bText = true;	
}


bool UGlyphTagsImportFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UDataAsset_FontTags *Object = Cast<UDataAsset_FontTags>(Obj);
	return IsValid(Object) && FactoryCanImport(Object->SourceFile);
};

void UGlyphTagsImportFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UDataAsset_FontTags *DataAsset_FontTags = Cast<UDataAsset_FontTags>(Obj);
	if(IsValid(DataAsset_FontTags))
	{
		DataAsset_FontTags->SourceFile = NewReimportPaths[0];
	}
}

bool UGlyphTagsImportFactory::FactoryCanImport(const FString& Filename)
{
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *Filename))
		return false;
	
	
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	const auto Reader = TJsonReaderFactory<>::Create(JsonString);
	FJsonSerializer::Deserialize(Reader, JsonObject);

	return JsonObject->HasField(TEXT("format")) && JsonObject->GetStringField(TEXT("format")) == "UnicodeBrowserGlyphTags_V1";
}


UObject* UGlyphTagsImportFactory::ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
{
	UDataAsset_FontTags *Object = Cast<UDataAsset_FontTags>(UFactory::ImportObject(InClass, InOuter, InName, InFlags, Filename, Parms, OutCanceled));
	if(!OutCanceled && IsValid(Object))
	{
		if(Object->ImportFromJson(Filename))
		{
			return Object;
		}
	}
	
	return Object;
}

EReimportResult::Type UGlyphTagsImportFactory::Reimport(UObject* Obj)
{
	UDataAsset_FontTags *DataAsset_FontTags = Cast<UDataAsset_FontTags>(Obj);
	return IsValid(DataAsset_FontTags) && DataAsset_FontTags->ImportFromJson(DataAsset_FontTags->SourceFile) ? EReimportResult::Type::Succeeded : EReimportResult::Type::Failed; 
};

UObject* UGlyphTagsImportFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn)
{
	return NewObject<UDataAsset_FontTags>(InParent, InName, Flags);	
}