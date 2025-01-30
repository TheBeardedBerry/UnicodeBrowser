// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "UObject/Object.h"
#include "GlyphTagsImportFactory.generated.h"

UCLASS()
class UNICODEBROWSER_API UGlyphTagsImportFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UGlyphTagsImportFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn) override;


	virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled) override;

	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames)  override;
	virtual bool FactoryCanImport(const FString& Filename) override;

	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;

	virtual EReimportResult::Type Reimport(UObject* Obj) override;

};
