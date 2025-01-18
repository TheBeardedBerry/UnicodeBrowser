#include "SUnicodeCharacterInfo.h"

#include "SlateOptMacros.h"

#include "UnicodeBrowser/UnicodeBrowserRow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeCharacterInfo::Construct(FArguments const& InArgs)
{
	SInvalidationPanel::Construct(SInvalidationPanel::FArguments());
	SetCanCache(true);
	SetRow(InArgs._Row);
}

void SUnicodeCharacterInfo::SetRow(TSharedPtr<FUnicodeBrowserRow> InRow)
{
	if(!InRow.IsValid())
		return;
	
	SetContent(
		SNew(SVerticalBox)
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Codepoint: 0x%04X"), InRow->Codepoint)))
			]
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Can Load: %s"), *LexToString(InRow->CanLoadCodepoint()))))
			]
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Size: %s"), *InRow->GetMeasurements().ToString())))
			]
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(InRow->GetFontData() ? FText::FromString(FString::Printf(TEXT("Font: %s"), *InRow->GetFontData()->GetFontFilename())) : FText::GetEmpty())
			]
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(InRow->GetFontData() ? FText::FromString(FString::Printf(TEXT("SubFace Index: %d"), InRow->GetFontData()->GetSubFaceIndex())) : FText::GetEmpty())
			]
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Scaling Factor: %3.3f"), InRow->GetScaling())))
			]		
		);
	
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
