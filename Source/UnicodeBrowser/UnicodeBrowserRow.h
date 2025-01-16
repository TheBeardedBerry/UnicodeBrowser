#pragma once
#include "Fonts/FontMeasure.h"
#include "Fonts/UnicodeBlockRange.h"

class FUnicodeBrowserRow : public TSharedFromThis<FUnicodeBrowserRow>
{
public:
	FUnicodeBrowserRow(int32 CodePointIn, TOptional<EUnicodeBlockRange> BlockRangeIn, FSlateFontInfo const *FontInfoIn = nullptr) : Codepoint(CodePointIn), BlockRange(BlockRangeIn), FontInfo(FontInfoIn)
	{		
		FUnicodeChar::CodepointToString(Codepoint, Character);
	}

	FString Character;			
	int32 Codepoint = 0;
	TOptional<EUnicodeBlockRange> BlockRange;	

private:
	FSlateFontInfo const *FontInfo = nullptr;
	const FFontData *FontData = nullptr;
	TOptional<float> ScalingFactor;
	TOptional<FVector2D> Measurements;
	TOptional<bool> bCanLoadCodepoint;

	
public:
	FFontData const* GetFontData()
	{
		if(!FontData && FontInfo)
		{
			float ScalingFactorResult;
			FontData = &FSlateApplication::Get().GetRenderer()->GetFontCache()->GetFontDataForCodepoint(*FontInfo, Codepoint, ScalingFactorResult);
			ScalingFactor = ScalingFactorResult;
		}
		
		return FontData;
	}

	bool CanLoadCodepoint()
	{
		if(!bCanLoadCodepoint.IsSet() && GetFontData()){
			bCanLoadCodepoint = FSlateApplication::Get().GetRenderer()->GetFontCache()->CanLoadCodepoint(*FontData, Codepoint);			
		}

		return bCanLoadCodepoint.Get(false);
	}

	FVector2D GetMeasurements()
	{
		if(!Measurements.IsSet() && FontInfo){	
			Measurements = FSlateApplication::Get().GetRenderer()->GetFontMeasureService()->Measure(*Character, *FontInfo);			
		}

		return Measurements.Get(FVector2D::ZeroVector);
	}

	float GetScaling()
	{
		if(!ScalingFactor.IsSet())
		{
			// this will try to populate the ScalingFactor
			GetFontData();
		}

		return ScalingFactor.Get(0.0f);
	}
	

	friend bool operator==(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS)
	{
		return Lhs.Codepoint == RHS.Codepoint
			&& Lhs.Character == RHS.Character
			&& Lhs.BlockRange == RHS.BlockRange
			&& Lhs.FontData == RHS.FontData
			&& Lhs.ScalingFactor == RHS.ScalingFactor
			&& Lhs.Measurements == RHS.Measurements
			&& Lhs.bCanLoadCodepoint == RHS.bCanLoadCodepoint;
	}

	friend bool operator!=(FUnicodeBrowserRow const& Lhs, FUnicodeBrowserRow const& RHS) { return !(Lhs == RHS); }
};
