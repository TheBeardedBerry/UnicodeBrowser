// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "Fonts/UnicodeBlockRange.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

#include "UnicodeBrowserWidget.generated.h"

class UFont;
class SScrollBox;
class SUbCheckBoxList;
class SCheckBoxList;
class SUniformGridPanel;

namespace UnicodeBrowser
{
	inline TOptional<EUnicodeBlockRange> GetUnicodeBlockRangeFromChar(int32 const CharCode);

	inline TArrayView<FUnicodeBlockRange const> GetUnicodeBlockRanges();

	inline int32 GetRangeIndex(EUnicodeBlockRange BlockRange)
	{
		return GetUnicodeBlockRanges().IndexOfByPredicate(
			[BlockRange](FUnicodeBlockRange const& Range)
			{
				return Range.Index == BlockRange;
			}
		);
	}

	static TArray<EUnicodeBlockRange> GetSymbolRanges();
}

class FUnicodeBrowserRow : public TSharedFromThis<FUnicodeBrowserRow>
{
public:
	FUnicodeBrowserRow(int32 const InCharCode, FString const& InCharacter, EUnicodeBlockRange InBlockRange)
		: CharCode(InCharCode),
		Character(InCharacter)
	{
		BlockRange = UnicodeBrowser::GetUnicodeBlockRangeFromChar(InCharCode);
	}

	int32 CharCode;
	FString Character;
	TOptional<EUnicodeBlockRange> BlockRange;

	friend bool operator==(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS)
	{
		return Lhs.CharCode == RHS.CharCode
			&& Lhs.Character == RHS.Character
			&& Lhs.BlockRange == RHS.BlockRange;
	}

	friend bool operator!=(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS) { return !(Lhs == RHS); }
};

UCLASS(Hidden, BlueprintType, EditInlineNew, DefaultToInstanced, DisplayName = "Font Options")
class UNICODEBROWSER_API UUnicodeBrowserOptions : public UObject
{
	GENERATED_BODY()
public:

	static TSharedRef<class IDetailsView> MakePropertyEditor(UUnicodeBrowserOptions* Options);
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties), Transient)
	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 18);

	UPROPERTY(EditAnywhere)
	int32 NumCols = 24;

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	DECLARE_MULTICAST_DELEGATE(FOnNumColsChangedDelegate);
	FOnNumColsChangedDelegate OnNumColsChanged;
	
};
/**
 * 
 */
class SUnicodeBrowserWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUnicodeBrowserWidget) {}
	SLATE_END_ARGS()

	void Construct(FArguments const& InArgs);

protected:

	TArray<TSharedPtr<FUnicodeBrowserRow>> Rows;
	TArray<TSharedPtr<SWidget>> RangeWidgets;
	TArrayView<FUnicodeBlockRange const> Ranges;
	TMap<EUnicodeBlockRange const, int32 const> RangeIndices;
	TMap<uint32, FString> SupportedCharacters;
	TObjectPtr<UUnicodeBrowserOptions> Options;
	TSharedPtr<SScrollBox> RangeScrollbox;
	TSharedPtr<SUbCheckBoxList> RangeSelector;
	TSharedPtr<class IDetailsView> FontDetailsView;
	mutable TMap<int32, TSharedRef<SBorder>> RowWidgetTextCache;
	mutable TSharedPtr<FUnicodeBrowserRow> CurrentRow;
	TSharedPtr<STextBlock> CurrentCharacterView;

protected:
	FReply OnCurrentCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent) const;
	FReply OnCharacterClicked(FGeometry const& Geometry, FPointerEvent const& PointerEvent, FString Character) const;
	FReply OnOnlySymbolsClicked();
	FReply OnRangeClicked(EUnicodeBlockRange BlockRange) const;
	FReply OnCharacterMouseMove(::FGeometry const& Geometry, ::FPointerEvent const& PointerEvent, TSharedPtr<FUnicodeBrowserRow> Row) const;
	FSlateFontInfo GetFont() const;
	FString GetUnicodeCharacterName(int32 CharCode);
	TSharedPtr<SUbCheckBoxList> MakeRangeSelector();
	TSharedPtr<SWidget> MakeRangeWidget(FUnicodeBlockRange Range) const;
	void PopulateSupportedCharacters();
	void RebuildGridColumns(::FUnicodeBlockRange Range, TSharedRef<SUniformGridPanel> const GridPanel) const;
};