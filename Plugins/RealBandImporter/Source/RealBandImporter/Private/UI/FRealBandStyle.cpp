// Copyright Epic Games, Inc. All Rights Reserved.
#include "FRealBandStyle.h"

#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

TUniquePtr<FSlateStyleSet> FRealBandStyle::MSStyleInstance;

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
	return Style;
}

FName FRealBandStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("RealBandStyle"));
	return StyleSetName;
}
