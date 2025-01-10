// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UnicodeBrowser/Widgets/UbSimpleExpander.h"

#include "SlateOptMacros.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUbSimpleExpander::Construct(const FArguments& InArgs)
{
	bIsExpanded = InArgs._IsExpanded;
	ShowExpandButton = InArgs._ShowExpandButton;
	ExpandedImage = FCoreStyle::Get().GetBrush("TreeArrow_Expanded");
	CollapsedImage = FCoreStyle::Get().GetBrush("TreeArrow_Collapsed");

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("DetailsView.GridLine"))
			.Padding(FMargin(0, 0, 0, 1))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryTop"))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "NoBorder")
						.OnClicked(this, &SUbSimpleExpander::ExpandButtonClicked)
						.Visibility(this, &SUbSimpleExpander::GetExpandButtonVisibility)
						.ForegroundColor(FSlateColor::UseForeground())
						[
							SNew(SImage)
							.Image(this, &SUbSimpleExpander::GetExpandButtonImage)
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
					+ SHorizontalBox::Slot()
					[
						InArgs._Header.Widget
					]
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.Visibility(this, &SUbSimpleExpander::GetBodyVisibility)
			[
				InArgs._Body.Widget
			]
		]
	];
}

FReply SUbSimpleExpander::ExpandButtonClicked()
{
	bIsExpanded = bIsExpanded.IsBound() ? bIsExpanded.Get() : !bIsExpanded.Get();// if bound, let the expansion be handled by the bound delegate
	if (bIsExpanded.IsBound())
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnExpansionChanged.ExecuteIfBound(!bIsExpanded.Get());
	}
	return FReply::Handled();
}

FSlateColor SUbSimpleExpander::GetBackgroundColor() const
{
	return IsHovered()
		       ? FAppStyle::Get().GetSlateColor("Colors.Header")
		       : FAppStyle::Get().GetSlateColor("Colors.Panel");
}

const FSlateBrush* SUbSimpleExpander::GetExpandButtonImage() const
{
	return bIsExpanded.Get() ? ExpandedImage : CollapsedImage;
}

EVisibility SUbSimpleExpander::GetBodyVisibility() const
{
	return  bIsExpanded.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SUbSimpleExpander::GetExpandButtonVisibility() const
{
	return ShowExpandButton.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
