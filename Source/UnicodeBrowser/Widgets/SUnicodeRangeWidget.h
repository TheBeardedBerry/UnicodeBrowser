// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Fonts/UnicodeBlockRange.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SUniformGridPanel.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeRangeWidget : public SExpandableArea
{
public:
	SLATE_BEGIN_ARGS(SUnicodeRangeWidget)
		{}
	//SLATE_ARGUMENT(SExpandableArea::FArguments, ParentArgs) 
	SLATE_ATTRIBUTE(FUnicodeBlockRange, Range);
	SLATE_END_ARGS()


	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FUnicodeBlockRange GetRange() const { return Range; }

	TSharedRef<SUniformGridPanel> GetGridPanel() const { return GridPanel.ToSharedRef(); };
	
private:
	FUnicodeBlockRange Range = FUnicodeBlockRange(EUnicodeBlockRange::ControlCharacter, INVTEXT("default"), FInt32Range(0, 1));
	TSharedPtr<SUniformGridPanel> GridPanel;
};
