// all rights reserved


#include "SUnicodeCharacterGridEntry.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeCharacterGridEntry::Construct(const FArguments& InArgs)
{
	UnicodeCharacter = InArgs._UnicodeCharacter.Get();

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(nullptr)
		.OnMouseMove(InArgs._OnMouseMove)
		.OnMouseDoubleClick(InArgs._OnMouseDoubleClick));
	
	if(!UnicodeCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[SUnicodeCharacterGridEntry::Construct] Widget created without valid CharacterRow pointer"));
		return;
	}
	
	ChildSlot [
		SAssignNew(TextBlock, STextBlock)
			.Font(InArgs._FontInfo)
			.IsEnabled(true)
			.Justification(ETextJustify::Center)
			.Text(FText::FromString(FString::Printf(TEXT("%s"), *UnicodeCharacter->Character)))
			.ToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X. Double-Click to copy: %s."), UnicodeCharacter->Codepoint, *UnicodeCharacter->Character)))
		];
	
}

void SUnicodeCharacterGridEntry::SetFontInfo(FSlateFontInfo& FontInfoIn)
{
	TextBlock->SetFont(FontInfoIn);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
