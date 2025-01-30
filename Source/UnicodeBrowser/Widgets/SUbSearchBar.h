#pragma once

#include "Framework/Application/SlateApplication.h"

#include "Widgets/Input/SSearchBox.h"

class UToolMenu;

class SUbSearchBar : public SSearchBox
{
	friend class SUnicodeBrowserWidget;

public:
	DECLARE_DELEGATE_OneParam(FOnTextChanged, FString)

	SLATE_BEGIN_ARGS(SUbSearchBar) {}
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);

	FOnTextChanged OnTextChanged;

protected:
	void TriggerUpdate() const
	{
		// this pretty much triggers an update as it will reevaluate the search
		OnTextChanged.ExecuteIfBound(GetText().ToString());
	}

	UToolMenu* CreateMenuSection_Settings() const;
};
