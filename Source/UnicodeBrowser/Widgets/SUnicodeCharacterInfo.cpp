#include "SUnicodeCharacterInfo.h"

#include "SlateOptMacros.h"

#include "UnicodeBrowser/UnicodeBrowserRow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SUnicodeCharacterInfo::Construct(FArguments const& InArgs)
{
	Row = InArgs._Row;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Codepoint: 0x%04X"), Row.Get()->Codepoint));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Can Load: %s"), *LexToString(Row.Get()->CanLoadCodepoint())));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Size: %s"), *Row.Get()->GetMeasurements().ToString()));
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					if (Row.Get()->GetFontData())
					{
						return FText::FromString(FString::Printf(TEXT("Font: %s"), *Row.Get()->GetFontData()->GetFontFilename()));
					}
					else
					{
						return FText::GetEmpty();
					}
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					if (Row.Get()->GetFontData())
					{
						return FText::FromString(FString::Printf(TEXT("SubFace Index: %d"), Row.Get()->GetFontData()->GetSubFaceIndex()));
					}
					else
					{
						return FText::GetEmpty();
					}
				}
			)
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text_Lambda(
				[this]()
				{
					return FText::FromString(FString::Printf(TEXT("Scaling Factor: %3.3f"), Row.Get()->GetScaling()));
				}
			)
		]
	];
}

TSharedPtr<FUnicodeBrowserRow> SUnicodeCharacterInfo::GetRow() const
{
	return Row.Get();
}

void SUnicodeCharacterInfo::SetRow(TSharedPtr<FUnicodeBrowserRow> InRow)
{
	Row = InRow;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
