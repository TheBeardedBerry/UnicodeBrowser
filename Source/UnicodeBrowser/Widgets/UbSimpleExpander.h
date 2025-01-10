// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once
#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "Modules/ModuleManager.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboButton.h"

DECLARE_DELEGATE_OneParam(FUbOnExpansionChanged, bool);

class SUbSimpleExpander : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUbSimpleExpander)
			: _IsExpanded(true), _ShowExpandButton(true)
		{
		}

		SLATE_ATTRIBUTE(bool, IsExpanded)
		SLATE_ATTRIBUTE(bool, ShowExpandButton)
		SLATE_NAMED_SLOT(FArguments, Header)
		SLATE_NAMED_SLOT(FArguments, Body)
		SLATE_EVENT(FUbOnExpansionChanged, OnExpansionChanged)

	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

private:
	const FSlateBrush* GetExpandButtonImage() const;
	EVisibility GetBodyVisibility() const;
	EVisibility GetExpandButtonVisibility() const;
	TAttribute<bool> ShowExpandButton = true;
	FReply ExpandButtonClicked();
	FSlateColor GetBackgroundColor() const;
	TAttribute<bool> bIsExpanded = false;
	FUbOnExpansionChanged OnExpansionChanged;
	const FSlateBrush* ExpandedImage = nullptr;
	const FSlateBrush* CollapsedImage = nullptr;
};
