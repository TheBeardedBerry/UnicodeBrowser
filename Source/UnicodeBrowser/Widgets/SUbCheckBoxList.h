// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateWidgetStyleAsset.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

enum class ECheckBoxState : uint8;
class ITableRow;
class SCheckBox;
class STableViewBase;

template <typename T>
class SListView;

namespace UbCheckBoxList
{
	struct FItemPair;
}

DECLARE_DELEGATE_OneParam(FOnCheckListItemStateChanged, int32);

/** A widget that can be used inside a CustomDialog to display a list of checkboxes */
class SUbCheckBoxList : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SUbCheckBoxList)
			: _CheckBoxStyle(&FCoreStyle::Get().GetWidgetStyle<FCheckBoxStyle>("Checkbox")), _IncludeGlobalCheckBoxInHeaderRow(true) {}

		/** The styling of the CheckBox */
		SLATE_STYLE_ARGUMENT(FCheckBoxStyle, CheckBoxStyle)
		/** The label of the item column header */
		SLATE_ARGUMENT(FText, ItemHeaderLabel)
		/** Optionally display a checkbox by the column header that toggles all items */
		SLATE_ARGUMENT(bool, IncludeGlobalCheckBoxInHeaderRow)
		/** Callback when any checkbox is changed. Parameter is the index of the item, or -1 if it was the "All"/Global checkbox */
		SLATE_EVENT(FOnCheckListItemStateChanged, OnItemCheckStateChanged)
	SLATE_END_ARGS()

public:
	void Construct(FArguments const& InArgs);
	void Construct(FArguments const& InArgs, TArray<FText> const& InItems, bool bIsChecked);
	void Construct(FArguments const& InArgs, TArray<TSharedRef<SWidget>> const& InItems, bool bIsChecked);

	int32 AddItem(FText const& Text, bool bIsChecked);
	int32 AddItem(TSharedRef<SWidget> Widget, bool bIsChecked);
	void UncheckAll();
	void RemoveAll();
	void RemoveItem(int32 Index);
	bool IsItemChecked(int32 Index) const;

	TArray<bool> GetValues() const;
	int32 GetNumCheckboxes() const;

	void UpdateAllChecked();
	ECheckBoxState GetToggleSelectedState() const;
	void OnToggleSelectedCheckBox(ECheckBoxState InNewState);
	void SetItemChecked(int32 Index, ECheckBoxState InNewState);
	void OnItemCheckBox(TSharedRef<UbCheckBoxList::FItemPair> const& InItem);

	TSharedRef<ITableRow> HandleGenerateRow(TSharedRef<UbCheckBoxList::FItemPair> InItem, TSharedRef<STableViewBase> const& OwnerTable);

	ECheckBoxState bAllCheckedState = ECheckBoxState::Unchecked;
	TArray<TSharedRef<UbCheckBoxList::FItemPair>> Items;

	FCheckBoxStyle const* CheckBoxStyle = nullptr;
	TSharedPtr<SListView<TSharedRef<UbCheckBoxList::FItemPair>>> ListView;

	FOnCheckListItemStateChanged OnItemCheckStateChanged;
};
