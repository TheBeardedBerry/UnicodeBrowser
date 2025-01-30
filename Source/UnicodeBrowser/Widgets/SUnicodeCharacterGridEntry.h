// all rights reserved
#pragma once

#include "CoreMinimal.h"

#include "UnicodeBrowser/UnicodeBrowserRow.h"

#include "Widgets/Layout/SBorder.h"

class STextBlock;

class UNICODEBROWSER_API SUnicodeCharacterGridEntry : public SBorder
{
public:
	DECLARE_DELEGATE_OneParam(FZoomEvent, float Offset)
	
	SLATE_BEGIN_ARGS(SUnicodeCharacterGridEntry) {}
		SLATE_ARGUMENT(SBorder::FArguments, ParentArgs)
		SLATE_ARGUMENT(FSlateFontInfo, FontInfo)
		SLATE_ATTRIBUTE(TSharedPtr<FUnicodeBrowserRow>, UnicodeCharacter)
		SLATE_EVENT(FPointerEventHandler, OnMouseMove)
		SLATE_EVENT(FPointerEventHandler, OnMouseDoubleClick)
		SLATE_EVENT(FZoomEvent, OnZoomFontSize)
		SLATE_EVENT(FZoomEvent, OnZoomCellPadding)
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);

	void SetFontInfo(FSlateFontInfo const& FontInfoIn);

	virtual void OnMouseEnter(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent) override;
	virtual void OnMouseLeave(FPointerEvent const& MouseEvent) override;
	virtual FReply OnMouseWheel(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(FGeometry const& MyGeometry, FPointerEvent const& MouseEvent) override;

private:
	TSharedPtr<FUnicodeBrowserRow> UnicodeCharacter;
	TSharedPtr<STextBlock> TextBlock;

	FZoomEvent OnZoomFontSize;
	FZoomEvent OnZoomCellPadding;
};
