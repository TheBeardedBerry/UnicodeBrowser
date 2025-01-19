// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "UnicodeBrowser/UnicodeBrowserWidget.h"

#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UNICODEBROWSER_API SUnicodeCharacterInfo : public SInvalidationPanel
{
public:
	SLATE_BEGIN_ARGS(SUnicodeCharacterInfo) {}
		SLATE_ARGUMENT(TSharedPtr<FUnicodeBrowserRow>, Row);
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);

	void SetRow(TWeakPtr<SUnicodeBrowserWidget> UnicodeBrowser, TSharedPtr<FUnicodeBrowserRow> InRow);
};
