// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SUnicodeBlockRangeSelector.h"
#include "SUnicodeCharacterInfo.h"
#include "Widgets/SCompoundWidget.h"

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
	void Construct(const FArguments& InArgs);
	
protected:
	TWeakPtr<class SUnicodeBrowserWidget> UnicodeBrowser;
	TSharedPtr<STextBlock> CurrentCharacterView;
	TSharedPtr<SUnicodeCharacterInfo> CurrentCharacterDetails;
	TSharedPtr<SUnicodeBlockRangeSelector> RangeSelector;
	
	void SelectAllRangesWithCharacters() const;
	TSharedRef<SExpandableArea> MakeBlockRangesSidebar();
};
