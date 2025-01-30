// Copyright Epic Games, Inc. All Rights Reserved.

#include "SUbCheckBoxList.h"

#include "SlateOptMacros.h"

#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"

#define LOCTEXT_NAMESPACE "SUbCheckBoxList"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace UbCheckBoxList
{
	FName const ColumnID_CheckBox("CheckBox");
	FName const ColumnID_Item("Item");

	constexpr float CheckBoxColumnWidth = 23.0f;

	class SItemPair : public SMultiColumnTableRow<TSharedRef<FItemPair>>
	{
	public:
		SLATE_BEGIN_ARGS(SItemPair) {}
			SLATE_STYLE_ARGUMENT(FCheckBoxStyle, CheckBoxStyle)
			SLATE_ARGUMENT(FSimpleDelegate, OnCheckUpdated)
		SLATE_END_ARGS()

	public:
		void Construct(FArguments const& InArgs, TSharedRef<STableViewBase> const& InOwner, TSharedRef<FItemPair> const& InItem)
		{
			CheckBoxStyle = InArgs._CheckBoxStyle;
			OnCheckUpdated = InArgs._OnCheckUpdated;
			Item = InItem;
			FSuperRowType::Construct(FTableRowArgs(), InOwner);
		}

	public:
		virtual TSharedRef<SWidget> GenerateWidgetForColumn(FName const& ColumnName) override
		{
			if (ColumnName == ColumnID_CheckBox)
			{
				return SNew(SCheckBox)
					.Style(CheckBoxStyle)
					.IsChecked(this, &SItemPair::GetToggleSelectedState)
					.OnCheckStateChanged(this, &SItemPair::OnToggleSelectedCheckBox);
			}
			else if (ColumnName == ColumnID_Item)
			{
				return Item->Widget;
			}
			check(false);
			return SNew(SCheckBox);
		}

		ECheckBoxState GetToggleSelectedState() const
		{
			return Item->bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}

		void OnToggleSelectedCheckBox(ECheckBoxState const InNewState) const
		{
			auto const bNewIsChecked = InNewState == ECheckBoxState::Checked;
			if (bNewIsChecked == Item->bIsChecked) return;
			Item->bIsChecked = bNewIsChecked;
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnCheckUpdated.ExecuteIfBound();
		}

		TSharedPtr<FItemPair> Item;
		FCheckBoxStyle const* CheckBoxStyle;
		FSimpleDelegate OnCheckUpdated;
	};
}

void SUbCheckBoxList::Construct(FArguments const& InArgs)
{
	Construct(InArgs, TArray<TSharedRef<SWidget>>(), false);
}

void SUbCheckBoxList::Construct(FArguments const& InArgs, TArray<FText> const& InItems, bool const bIsChecked)
{
	TArray<TSharedRef<SWidget>> Widgets;
	Widgets.Reserve(InItems.Num());
	for (FText const& Text : InItems)
	{
		Widgets.Add(SNew(STextBlock).Text(Text));
	}
	Construct(InArgs, Widgets, bIsChecked);
}

void SUbCheckBoxList::Construct(FArguments const& InArgs, TArray<TSharedRef<SWidget>> const& InItems, bool bIsChecked)
{
	CheckBoxStyle = InArgs._CheckBoxStyle;

	Items.Reserve(InItems.Num());
	for (TSharedRef<SWidget> Widget : InItems)
	{
		Items.Add(MakeShared<UbCheckBoxList::FItemPair>(Widget, bIsChecked));
	}

	bool bShowHeaderCheckbox = InArgs._IncludeGlobalCheckBoxInHeaderRow;
	OnItemCheckStateChanged = InArgs._OnItemCheckStateChanged;

	TSharedRef<SHeaderRow> const HeaderRowWidget = SNew(SHeaderRow)
		+ SHeaderRow::Column(UbCheckBoxList::ColumnID_CheckBox)
		.FixedWidth(UbCheckBoxList::CheckBoxColumnWidth)
		[
			SNew(SCheckBox)
			.Style(InArgs._CheckBoxStyle)
			.IsChecked(this, &SUbCheckBoxList::GetAllCheckedState)
			.OnCheckStateChanged(this, &SUbCheckBoxList::OnAllCheckedStateChanged)
			.Visibility(InArgs._IncludeGlobalCheckBoxInHeaderRow ? EVisibility::Visible : EVisibility::Hidden)
			//.Visibility_Lambda([bShowHeaderCheckbox] { return bShowHeaderCheckbox ? EVisibility::Visible : EVisibility::Hidden; })
		]
		+ SHeaderRow::Column(UbCheckBoxList::ColumnID_Item)
		.DefaultLabel(InArgs._ItemHeaderLabel)
		.FillWidth(1.0f);

	ChildSlot
	[
		SAssignNew(ListView, SListView<TSharedRef<UbCheckBoxList::FItemPair>>)
		.ListItemsSource(&ItemsFiltered)
		.OnGenerateRow(this, &SUbCheckBoxList::HandleGenerateRow)
		.HeaderRow(HeaderRowWidget)
		.SelectionMode(ESelectionMode::None)
		.OnItemsRebuilt(this, &SUbCheckBoxList::OnItemsRebuilt)
	];
}

int32 SUbCheckBoxList::AddItem(FText const& Text, bool bIsChecked)
{
	return AddItem(MakeShared<UbCheckBoxList::FItemPair>(SNew(STextBlock).Text(Text), bIsChecked));
}

int32 SUbCheckBoxList::AddItem(TSharedRef<SWidget> Widget, bool bIsChecked)
{
	return AddItem(MakeShared<UbCheckBoxList::FItemPair>(Widget, bIsChecked));
}

int32 SUbCheckBoxList::AddItem(TSharedRef<UbCheckBoxList::FItemPair> Item)
{
	int32 const ReturnValue = Items.Add(Item);
	UpdateItems();
	return ReturnValue;
}

void SUbCheckBoxList::UncheckAll()
{
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		SetItemChecked(Index, ECheckBoxState::Unchecked);
	}
}

void SUbCheckBoxList::RemoveAll()
{
	Items.Reset();
	UpdateItems();
}

void SUbCheckBoxList::RemoveItem(int32 const Index)
{
	if (Items.IsValidIndex(Index))
	{
		Items.RemoveAt(Index);
		UpdateItems();
	}
}

bool SUbCheckBoxList::IsItemChecked(int32 const Index) const
{
	return Items.IsValidIndex(Index) ? Items[Index]->bIsChecked : false;
}

TArray<bool> SUbCheckBoxList::GetValues() const
{
	TArray<bool> Values;
	for (TSharedRef<UbCheckBoxList::FItemPair> const& Item : Items)
	{
		Values.Add(Item->bIsChecked);
	}
	return Values;
}

int32 SUbCheckBoxList::GetNumCheckboxes() const
{
	return Items.Num();
}

void SUbCheckBoxList::UpdateAllChecked()
{
	bool const bContainsTrue = Items.ContainsByPredicate([](TSharedRef<UbCheckBoxList::FItemPair> const& Item) { return Item->bIsChecked; });
	bool const bContainsFalse = Items.ContainsByPredicate([](TSharedRef<UbCheckBoxList::FItemPair> const& Item) { return !Item->bIsChecked; });
	bAllCheckedState = bContainsTrue && bContainsFalse ? ECheckBoxState::Undetermined : (bContainsTrue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

void SUbCheckBoxList::UpdateItems()
{
	ItemsFiltered = Items.FilterByPredicate([](TSharedRef<UbCheckBoxList::FItemPair> const &Item){ return Item->bIsVisible; });
	ListView->RebuildList();
}

ECheckBoxState SUbCheckBoxList::GetAllCheckedState() const
{
	return bAllCheckedState;
}

void SUbCheckBoxList::OnAllCheckedStateChanged(ECheckBoxState const InNewState)
{
	bool const bNewValue = InNewState == ECheckBoxState::Checked;
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		SetItemChecked(Index, InNewState);
	}
	bAllCheckedState = bNewValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	UpdateItems();

	// ReSharper disable once CppExpressionWithoutSideEffects
	OnItemCheckStateChanged.ExecuteIfBound(-1);
}

void SUbCheckBoxList::OnItemsRebuilt()
{
	UpdateAllChecked();
}

void SUbCheckBoxList::SetItemChecked(int32 const Index, ECheckBoxState const InNewState)
{
	if (Items.IsValidIndex(Index))
	{
		auto const Item = Items[Index];
		bool const bNewIsChecked = InNewState == ECheckBoxState::Checked;
		if (Item->bIsChecked == bNewIsChecked) return;
		Item->bIsChecked = bNewIsChecked;
		OnItemCheckBoxChanged(Item);
		UpdateItems();
	}
}

void SUbCheckBoxList::SetItemVisibility(int32 const Index, bool const bVisible)
{
	if (Items.IsValidIndex(Index))
	{
		auto const Item = Items[Index];
		if (Item->bIsVisible == bVisible) return;
		Item->bIsVisible = bVisible;
		UpdateItems();
	}
}

void SUbCheckBoxList::OnItemCheckBoxChanged(TSharedRef<UbCheckBoxList::FItemPair> const& InItem)
{
	UpdateAllChecked();
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnItemCheckStateChanged.ExecuteIfBound(Items.IndexOfByKey(InItem));
}

TSharedRef<ITableRow> SUbCheckBoxList::HandleGenerateRow(TSharedRef<UbCheckBoxList::FItemPair> InItem, TSharedRef<STableViewBase> const& OwnerTable)
{
	return SNew(UbCheckBoxList::SItemPair, OwnerTable, InItem)
		.CheckBoxStyle(CheckBoxStyle)
		.OnCheckUpdated(FSimpleDelegate::CreateLambda([this, InItem]() { OnItemCheckBoxChanged(InItem); }));
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
