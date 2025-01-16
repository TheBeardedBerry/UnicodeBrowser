// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UnicodeBrowser/UnicodeBrowserWidget.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeCharacterInfo : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SUnicodeCharacterInfo) {}
	SLATE_ATTRIBUTE(TSharedPtr<FUnicodeBrowserRow>, Row);
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);
	
	TAttribute<TSharedPtr<FUnicodeBrowserRow>> Row;
	TSharedPtr<FUnicodeBrowserRow> GetRow() const;
	
	void SetRow(TSharedPtr<FUnicodeBrowserRow> InRow);
};
