#include "SUnicodeCharacterInfo.h"

#include "SlateOptMacros.h"

#include "UnicodeBrowser/UnicodeBrowserRow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeCharacterInfo::Construct(FArguments const& InArgs)
{
	SInvalidationPanel::Construct(SInvalidationPanel::FArguments());
	SetCanCache(true);
	SetRow(nullptr, InArgs._Row);
}

void SUnicodeCharacterInfo::SetRow(TWeakPtr<SUnicodeBrowserWidget> UnicodeBrowser, TSharedPtr<FUnicodeBrowserRow> InRow)
{
	if(!InRow.IsValid())
		return;

	FText TagsText = FText::GetEmpty();
	
	if(UnicodeBrowser.IsValid())
	{
		if(UnicodeBrowser.Pin().Get()->Options->Preset && UnicodeBrowser.Pin().Get()->Options->Preset->SupportsFont(*InRow->FontInfo))
		{
			TagsText = FText::FromString(TEXT("Tags: ") + FString::Join(UnicodeBrowser.Pin().Get()->Options->Preset->GetCodepointTags(InRow->Codepoint), TEXT(", ")));			
		}
	}
	
	SetContent(
		SNew(SVerticalBox)
			// codepoint
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Codepoint: 0x%04X"), InRow->Codepoint)))
			]
			// can load
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Can Load: %s"), *LexToString(InRow->CanLoadCodepoint()))))
			]
			// dimensions
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Size: %s"), *InRow->GetMeasurements().ToString())))
			]
			// font object
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(InRow->GetFontData() ? FText::FromString(FString::Printf(TEXT("Font: %s"), *InRow->GetFontData()->GetFontFilename())) : FText::GetEmpty())
			]
			// subface index
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(InRow->GetFontData() ? FText::FromString(FString::Printf(TEXT("SubFace Index: %d"), InRow->GetFontData()->GetSubFaceIndex())) : FText::GetEmpty())
			]
			// scaling factor
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("Scaling Factor: %3.3f"), InRow->GetScaling())))
			]
			// tags
			+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(TagsText)
			]
		);
	
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
