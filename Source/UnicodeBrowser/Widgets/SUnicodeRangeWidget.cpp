// all rights reserved


#include "SUnicodeRangeWidget.h"

#include "SlateOptMacros.h"
#include "Widgets/Layout/SScaleBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeRangeWidget::Construct(const FArguments& InArgs)
{
	Range = InArgs._Range.Get();

	SExpandableArea::Construct(SExpandableArea::FArguments()
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
	);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
