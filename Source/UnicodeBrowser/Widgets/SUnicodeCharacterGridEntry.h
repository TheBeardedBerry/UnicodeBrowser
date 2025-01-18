// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowser/UnicodeBrowserRow.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeCharacterGridEntry : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SUnicodeCharacterGridEntry) {}
		SLATE_ARGUMENT(SBorder::FArguments, ParentArgs)
		SLATE_ARGUMENT(FSlateFontInfo, FontInfo)
		SLATE_ATTRIBUTE(TSharedPtr<FUnicodeBrowserRow>, UnicodeCharacter)
		SLATE_EVENT(FPointerEventHandler, OnMouseMove)
		SLATE_EVENT(FPointerEventHandler, OnMouseDoubleClick)	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetFontInfo(FSlateFontInfo &FontInfoIn);

private:
	TSharedPtr<FUnicodeBrowserRow> UnicodeCharacter;
	TSharedPtr<STextBlock> TextBlock;
};
