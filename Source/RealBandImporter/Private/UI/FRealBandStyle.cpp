// Copyright Epic Games, Inc. All Rights Reserved.
#include "FRealBandStyle.h"

#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/AppStyle.h"

TUniquePtr<FSlateStyleSet> FRealBandStyle::MSStyleInstance;
FString PluginPath = FPaths::Combine(FPaths::EnginePluginsDir(), TEXT("RealBandImporter"));

//#define TTF_FONT(RelativePath, ...) FSlateFontInfo(FPaths::EngineContentDir() / "Resources" / RelativePath + TEXT(".ttf"), __VA_ARGS__)
#define TTF_FONT(RelativePath, ...) FSlateFontInfo(FPaths::Combine(TEXT("RealBandImporter"), TEXT("Resources"), TEXT("*.otf")), __VA_ARGS__)
//#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::Combine(TEXT("RealBandImporter"), TEXT("Resources"), TEXT(".png") ), __VA_ARGS__ )

void FRealBandStyle::Initialize()
{
	if (!MSStyleInstance.IsValid())
	{
		MSStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*MSStyleInstance);
	}
}

TUniquePtr< FSlateStyleSet > FRealBandStyle::Create()
{
	TUniquePtr< FSlateStyleSet > Style = MakeUnique<FSlateStyleSet>(GetStyleSetName());
	Style->SetContentRoot(FPaths::EnginePluginsDir() / TEXT("RealBandImporter/Resources"));
	//Style->Set("MyFontStyle", FWindowStyle()
    //.SetBackgroundBrush(IMAGE_BRUSH("brush", FVector2D(912, 582), FLinearColor(FColor(0.4352F,0.3647F,0.8862F)), ESlateBrushTileType::NoTile))
	Style->Set("MyFontStyle", FTextBlockStyle(FTextBlockStyle::GetDefault())

		.SetColorAndOpacity(FSlateColor(FLinearColor(100.4352f, 100.3647f, 100.8862f)))
		.SetFont(TTF_FONT("Pretendard-Black", 40)));
	//Style->SetBackgroundBrush(IMAGE_BRUSH("brush", FVector2D(912, 582), FLinearColor(FColor(0.4352F, 0.3647F, 0.8862F)), ESlateBrushTileType::NoTile));
	return Style;
}

FName FRealBandStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("RealBandStyle"));
	return StyleSetName;
}


const ISlateStyle& FRealBandStyle::Get()
{
	check(MSStyleInstance);
	return *MSStyleInstance;
}