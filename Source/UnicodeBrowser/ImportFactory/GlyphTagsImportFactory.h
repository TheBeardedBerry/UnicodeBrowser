// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"

#include "Factories/Factory.h"

#include "UObject/Object.h"

#include "GlyphTagsImportFactory.generated.h"

UCLASS()
class UNICODEBROWSER_API UGlyphTagsImportFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UGlyphTagsImportFactory(FObjectInitializer const& ObjectInitializer);

	virtual UObject* FactoryCreateText(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		TCHAR const* Type,
		TCHAR const*& Buffer,
		TCHAR const* BufferEnd,
		FFeedbackContext* Warn
	) override;

	virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, FString const& Filename, TCHAR const* Parms, bool& OutCanceled) override;

	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual bool FactoryCanImport(FString const& Filename) override;

	virtual void SetReimportPaths(UObject* Obj, TArray<FString> const& NewReimportPaths) override;

	virtual EReimportResult::Type Reimport(UObject* Obj) override;
};
