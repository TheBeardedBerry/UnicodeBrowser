// all rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SUnicodeCharacterGridEntry.h"
#include "Fonts/UnicodeBlockRange.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SUniformGridPanel.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeRangeWidget : public SBorder
{
public:
	DECLARE_DELEGATE_OneParam(FZoomEvent, float Offset)
	
	SLATE_BEGIN_ARGS(SUnicodeRangeWidget)
		{}
	SLATE_ATTRIBUTE(FUnicodeBlockRange, Range);
	SLATE_EVENT(FZoomEvent, OnZoomFontSize)
	SLATE_EVENT(FZoomEvent, OnZoomColumnCount)
	SLATE_END_ARGS()

	TArray<TSharedPtr<SUnicodeCharacterGridEntry>> Characters;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FUnicodeBlockRange GetRange() const { return Range; }

	TSharedRef<SUniformGridPanel> GetGridPanel() const { return GridPanel.ToSharedRef(); };
	
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;;
	
private:
	FUnicodeBlockRange Range = FUnicodeBlockRange(EUnicodeBlockRange::ControlCharacter, INVTEXT("default"), FInt32Range(0, 1));
	TSharedPtr<SUniformGridPanel> GridPanel;
	FZoomEvent OnZoomFontSize;
	FZoomEvent OnZoomColumnCount;
};
