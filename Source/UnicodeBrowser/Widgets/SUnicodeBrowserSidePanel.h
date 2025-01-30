// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SUnicodeBlockRangeSelector.h"
#include "SUnicodeCharacterInfo.h"

#include "Widgets/Layout/SSplitter.h"

class SExpandableArea;
class STextBlock;
/**
 * 
 */
class UNICODEBROWSER_API SUnicodeBrowserSidePanel : public SSplitter
{
	friend class SUnicodeBrowserWidget;
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserSidePanel){}
	SLATE_ARGUMENT(TWeakPtr<class SUnicodeBrowserWidget>, UnicodeBrowser)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(FArguments const& InArgs);
	
protected:
	TWeakPtr<class SUnicodeBrowserWidget> UnicodeBrowser;
	TSharedPtr<STextBlock> CurrentCharacterView;
	TSharedPtr<SUnicodeCharacterInfo> CurrentCharacterDetails;
	TSharedPtr<SUnicodeBlockRangeSelector> RangeSelector;

	/* @param Rows The set which should be evaluated when checking for entries
	 * @param bExclusive Should all other ranges be disabled? */
	void SelectAllRangesWithCharacters(TMap<EUnicodeBlockRange, TArray<TSharedPtr<FUnicodeBrowserRow>>> &Rows, bool bExclusive = true) const;
	TSharedRef<SExpandableArea> MakeBlockRangesSidebar();
};
