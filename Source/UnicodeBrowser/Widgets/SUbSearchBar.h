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
	
	protected:
		TSharedPtr<SSearchBox> SearchBox;
		TSharedPtr<SCheckBox> CheckBox_AutoSetRanges;
		TSharedPtr<SCheckBox> CheckBox_CaseSensitive;


		void TriggerUpdate()
		{
			// this pretty much triggers an update as it will reevaluate the search
			OnTextChanged.ExecuteIfBound(SearchBox->GetText().ToString());
		}
};