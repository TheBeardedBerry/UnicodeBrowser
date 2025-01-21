// all rights reserved
#include "SUnicodeCharacterGridEntry.h"

#include "SlateOptMacros.h"
#include "HAL/PlatformApplicationMisc.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeCharacterGridEntry::Construct(const FArguments& InArgs)
{
	UnicodeCharacter = InArgs._UnicodeCharacter.Get();

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(nullptr)
		.OnMouseMove(InArgs._OnMouseMove)
	);		
	
	if(!UnicodeCharacter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SUnicodeCharacterGridEntry::Construct] Widget created without valid CharacterRow pointer"));
		return;
	}

	ChildSlot [
		SAssignNew(TextBlock, STextBlock)
			.Font(InArgs._FontInfo)
			.IsEnabled(true)
			.Justification(ETextJustify::Center)
			.Text(FText::FromString(*UnicodeCharacter->Character))
			.Visibility(EVisibility::HitTestInvisible)
		];
	
}

void SUnicodeCharacterGridEntry::SetFontInfo(FSlateFontInfo& FontInfoIn)
{
	TextBlock->SetFont(FontInfoIn);
}

void SUnicodeCharacterGridEntry::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SetToolTipText(FText::FromString(FString::Printf(TEXT("Char Code: U+%-06.04X.\nDouble-Click to copy: %s."), UnicodeCharacter->Codepoint, *UnicodeCharacter->Character)));
	SBorder::OnMouseEnter(MyGeometry, MouseEvent);
	SetColorAndOpacity(FLinearColor(0, 0.44, 0.88));
}

void SUnicodeCharacterGridEntry::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SBorder::OnMouseLeave(MouseEvent);
	SetColorAndOpacity(FLinearColor::White);
}

FReply SUnicodeCharacterGridEntry::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SBorder::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
	// the color and tooltip get reset by MouseEnter/MouseLeave 
	SetColorAndOpacity(FLinearColor(0.35, 1.0, 0.35, 1.0));
	SetToolTipText(FText::FromString(FString::Printf(TEXT("Character copied to clipboard"), UnicodeCharacter->Codepoint, *UnicodeCharacter->Character)));
	FPlatformApplicationMisc::ClipboardCopy(*UnicodeCharacter->Character);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
