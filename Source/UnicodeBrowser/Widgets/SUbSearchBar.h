#pragma once

#include "Widgets/Input/SSearchBox.h"
#include "Framework/Application/SlateApplication.h"


class SUbSearchBar : public SBox {
	friend class SUnicodeBrowserWidget;
	
	public:
		DECLARE_DELEGATE_OneParam(FOnTextChanged, FString)
	
		SLATE_BEGIN_ARGS(SUbSearchBar){}
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
	
		FOnTextChanged OnTextChanged;

		bool bAutoSetRange = true;
		bool bCaseSensitive = false;
	
	protected:
		TSharedPtr<SSearchBox> SearchBox;


		void TriggerUpdate()
		{
			// this pretty much triggers an update as it will reevaluate the search
			OnTextChanged.ExecuteIfBound(SearchBox->GetText().ToString());
		}
};