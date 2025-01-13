// Copyright Epic Games, Inc. All Rights Reserved.

#include "SUbCheckBoxList.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SUbCheckBoxList"

namespace UbCheckBoxList
{
	FName const ColumnID_CheckBox("CheckBox");
	FName const ColumnID_Item("Item");

	float const CheckBoxColumnWidth = 23.0f;

	struct FItemPair
	{
		TSharedRef<SWidget> Widget;
		
		bool bIsChecked;
		FItemPair(TSharedRef<SWidget> InWidget, bool const bInChecked)
			: Widget(InWidget), bIsChecked(bInChecked)
		{ }
	};

	class SItemPair : public SMultiColumnTableRow<TSharedRef<FItemPair>>
	{
	public:
		SLATE_BEGIN_ARGS(SItemPair) { }
			SLATE_STYLE_ARGUMENT(FCheckBoxStyle, CheckBoxStyle)
			SLATE_ARGUMENT(FSimpleDelegate, OnCheckUpdated)
		SLATE_END_ARGS()

	public:
		void Construct(FArguments const& InArgs, TSharedRef<STableViewBase> const& InOwner, TSharedRef<FItemPair> InItem)
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

		void OnToggleSelectedCheckBox(ECheckBoxState const InNewState)
		{
			Item->bIsChecked = InNewState == ECheckBoxState::Checked;
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
		[
			SNew(SCheckBox)
			.Style(InArgs._CheckBoxStyle)
			.IsChecked(this, &SUbCheckBoxList::GetToggleSelectedState)
			.OnCheckStateChanged(this, &SUbCheckBoxList::OnToggleSelectedCheckBox)
			.Visibility_Lambda([bShowHeaderCheckbox] { return bShowHeaderCheckbox ? EVisibility::Visible : EVisibility::Hidden; })
		]
		.FixedWidth(UbCheckBoxList::CheckBoxColumnWidth)
		+ SHeaderRow::Column(UbCheckBoxList::ColumnID_Item)
		.DefaultLabel(InArgs._ItemHeaderLabel)
		.FillWidth(1.0f);

	ChildSlot
	[
		SAssignNew(ListView, SListView<TSharedRef<UbCheckBoxList::FItemPair>>)
		.ListItemsSource(&Items)
		.OnGenerateRow(this, &SUbCheckBoxList::HandleGenerateRow)
		.HeaderRow(HeaderRowWidget)
		.SelectionMode(ESelectionMode::None)
	];
}

int32 SUbCheckBoxList::AddItem(FText const& Text, bool bIsChecked)
{
	int32 const ReturnValue = Items.Add(MakeShared<UbCheckBoxList::FItemPair>(SNew(STextBlock).Text(Text), bIsChecked));
	ListView->RebuildList();
	return ReturnValue;
}

int32 SUbCheckBoxList::AddItem(TSharedRef<SWidget> Widget, bool bIsChecked)
{
	int32 const ReturnValue = Items.Add(MakeShared<UbCheckBoxList::FItemPair>(Widget, bIsChecked));
	ListView->RebuildList();
	return ReturnValue;
}

void SUbCheckBoxList::UncheckAll()
{
	for (TSharedRef<UbCheckBoxList::FItemPair> const& Item : Items)
	{
		Item->bIsChecked = false;
	}
	ListView->RebuildList();
}
void SUbCheckBoxList::RemoveAll()
{
	Items.Reset();
	ListView->RebuildList();
}

void SUbCheckBoxList::RemoveItem(int32 const Index)
{
	if (Items.IsValidIndex(Index))
	{
		Items.RemoveAt(Index);
		ListView->RebuildList();
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

ECheckBoxState SUbCheckBoxList::GetToggleSelectedState() const
{
	return bAllCheckedState;
}

void SUbCheckBoxList::OnToggleSelectedCheckBox(ECheckBoxState const InNewState)
{
	bool const bNewValue = InNewState == ECheckBoxState::Checked;
	for (TSharedRef<UbCheckBoxList::FItemPair> const& Item : Items)
	{
		Item->bIsChecked = bNewValue;
	}
	bAllCheckedState = bNewValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnItemCheckStateChanged.ExecuteIfBound(-1);
}

void SUbCheckBoxList::SetItemChecked(int32 const Index, ECheckBoxState const InNewState)
{
	if (Items.IsValidIndex(Index))
	{
		Items[Index]->bIsChecked = InNewState == ECheckBoxState::Checked;
		ListView->RebuildList();
	}
}

void SUbCheckBoxList::OnItemCheckBox(TSharedRef<UbCheckBoxList::FItemPair> InItem)
{
	UpdateAllChecked();
	// ReSharper disable once CppExpressionWithoutSideEffects
	OnItemCheckStateChanged.ExecuteIfBound(Items.IndexOfByKey(InItem));
}

TSharedRef<ITableRow> SUbCheckBoxList::HandleGenerateRow(TSharedRef<UbCheckBoxList::FItemPair> InItem, TSharedRef<STableViewBase> const& OwnerTable)
{
	return SNew(UbCheckBoxList::SItemPair, OwnerTable, InItem)
		.CheckBoxStyle(CheckBoxStyle)
		.OnCheckUpdated(FSimpleDelegate::CreateLambda([this, InItem]() { OnItemCheckBox(InItem); }));
	
}

#undef LOCTEXT_NAMESPACE