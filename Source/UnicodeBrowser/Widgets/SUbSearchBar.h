#pragma once

#include "Widgets/Input/SSearchBox.h"
#include "Framework/Application/SlateApplication.h"


class SUbSearchBar : public SSearchBox {
	friend class SUnicodeBrowserWidget;
	
	public:
		DECLARE_DELEGATE_OneParam(FOnTextChanged, FString)
	
		SLATE_BEGIN_ARGS(SUbSearchBar){}
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	
		FOnTextChanged OnTextChanged;		
	
	protected:

		void TriggerUpdate()
		{
			// this pretty much triggers an update as it will reevaluate the search
			OnTextChanged.ExecuteIfBound(GetText().ToString());
		}

		UToolMenu* CreateMenuSection_Settings();
};