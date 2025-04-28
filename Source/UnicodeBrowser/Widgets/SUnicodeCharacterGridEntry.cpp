// all rights reserved

#include "SUnicodeCharacterGridEntry.h"

#include "SlateOptMacros.h"

#include "HAL/PlatformApplicationMisc.h"

#include "UnicodeBrowser/UnicodeBrowserOptions.h"

#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FSlateBrush SUnicodeCharacterGridEntry::BorderBrush = FSlateBrush();

void SUnicodeCharacterGridEntry::Construct(FArguments const& InArgs)
{
	UnicodeCharacter = InArgs._UnicodeCharacter.Get();

	OnZoomFontSize = InArgs._OnZoomFontSize;
	OnZoomCellPadding = InArgs._OnZoomCellPadding;

	// idk if there's a nicer way to initialize the static brush... once.
	BorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	BorderBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	BorderBrush.OutlineSettings.CornerRadii = FVector4(10.0, 10.0, 10.0, 10.0);
	
	SBorder::Construct(
		SBorder::FArguments()
		.BorderBackgroundColor(FLinearColor::Transparent)
		.BorderImage(&BorderBrush)
		.OnMouseMove(InArgs._OnMouseMove)
		.VAlign(VAlign_Center)
	);

	if (!UnicodeCharacter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SUnicodeCharacterGridEntry::Construct] Widget created without valid CharacterRow pointer"));
		return;
	}

	ChildSlot
	[
		SAssignNew(TextBlock, STextBlock)
		.Font(InArgs._FontInfo)
		.IsEnabled(true)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString(*UnicodeCharacter->Character))
		.Visibility(EVisibility::HitTestInvisible)
		.Margin(UUnicodeBrowserOptions::Get()->GridCellPadding)
	];
}

void SUnicodeCharacterGridEntry::SetFontInfo(FSlateFontInfo const& FontInfoIn)
{
	TextBlock->SetFont(FontInfoIn);
	Invalidate(EInvalidateWidgetReason::Layout);
}

void SUnicodeCharacterGridEntry::OnMouseEnter(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent)
{
	SetToolTipText(FText::GetEmpty());
	SBorder::OnMouseEnter(MyGeometry, MouseEvent);
	SetBorderBackgroundColor(FLinearColor(0, 0.44, 0.88, 0.1));
}

void SUnicodeCharacterGridEntry::OnMouseLeave(FPointerEvent const& MouseEvent)
{
	SBorder::OnMouseLeave(MouseEvent);
	SetBorderBackgroundColor(FLinearColor::Transparent);
}

FReply SUnicodeCharacterGridEntry::OnMouseWheel(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent)
{
	if (MouseEvent.GetWheelDelta() && MouseEvent.IsControlDown())
	{
		// CTRL + !Shift => Zoom Font
		if (!MouseEvent.IsShiftDown() && OnZoomFontSize.IsBound())
		{
			OnZoomFontSize.Execute(MouseEvent.GetWheelDelta());
			return FReply::Handled();
		}

		// CTRL + Shift => Zoom Columns
		if (MouseEvent.IsShiftDown() && OnZoomCellPadding.IsBound())
		{
			OnZoomCellPadding.Execute(MouseEvent.GetWheelDelta());
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply SUnicodeCharacterGridEntry::OnMouseButtonDoubleClick(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent)
{
	SBorder::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
	// the color and tooltip get reset by MouseEnter/MouseLeave 
	SetBorderBackgroundColor(FLinearColor(0.35, 1.0, 0.35, 0.2));
	SetToolTipText(FText::FromString(FString::Printf(TEXT("Character copied to clipboard"))));
	FPlatformApplicationMisc::ClipboardCopy(*UnicodeCharacter->Character);
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
