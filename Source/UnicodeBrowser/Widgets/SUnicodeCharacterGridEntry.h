// all rights reserved
#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowser/UnicodeBrowserRow.h"
#include "Widgets/SCompoundWidget.h"

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

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetFontInfo(FSlateFontInfo &FontInfoIn);

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	TSharedPtr<FUnicodeBrowserRow> UnicodeCharacter;
	TSharedPtr<STextBlock> TextBlock;

	
	FZoomEvent OnZoomFontSize;
	FZoomEvent OnZoomCellPadding;
};
