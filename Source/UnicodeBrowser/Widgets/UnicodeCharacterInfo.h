// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "../UnicodeBrowserRow.h"

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
