// all rights reserved


#include "SUnicodeRangeWidget.h"

#include "SlateOptMacros.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeRangeWidget::Construct(const FArguments& InArgs)
{
	Range = InArgs._Range.Get();
	OnZoomFontSize = InArgs._OnZoomFontSize;
	OnZoomColumnCount = InArgs._OnZoomColumnCount;
	
	SBorder::Construct(SBorder::FArguments());

	ChildSlot [
		SNew(SExpandableArea)
			.HeaderPadding(FMargin(2, 4))
			.HeaderContent()
			[
				SNew(STextBlock)
				.Text(Range.DisplayName)
			]
			.BodyContent()
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFit)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(GridPanel, SUniformGridPanel)
					.SlotPadding(FMargin(6.f, 4.f))
				]
			]	
	];
}

FReply SUnicodeRangeWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(MouseEvent.GetWheelDelta() && MouseEvent.IsControlDown())
	{
		// CTRL + !Shift => Zoom Font
		if(!MouseEvent.IsShiftDown() && OnZoomFontSize.IsBound())
		{
			OnZoomFontSize.Execute(MouseEvent.GetWheelDelta());			
			return FReply::Handled();
		}

		// CTRL + Shift => Zoom Columns
		if(MouseEvent.IsShiftDown() && OnZoomColumnCount.IsBound())
		{
			OnZoomColumnCount.Execute(MouseEvent.GetWheelDelta());			
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
