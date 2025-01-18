// all rights reserved

#pragma once

#include "CoreMinimal.h"

#include "UnicodeBrowser/UnicodeBrowserRow.h"

#include "Widgets/Layout/SBorder.h"

class STextBlock;
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

	void Construct(FArguments const& InArgs);

	void SetFontInfo(FSlateFontInfo const& FontInfoIn);

private:
	TSharedPtr<FUnicodeBrowserRow> UnicodeCharacter;
	TSharedPtr<STextBlock> TextBlock;
};
