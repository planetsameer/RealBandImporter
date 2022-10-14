// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class FSlateStyleSet;
class ISlateStyle;
class FRealBandStyle
{
public:
	static void Initialize();
	static FName GetStyleSetName();
	static const  ISlateStyle& Get(); 
private:
	static TUniquePtr<FSlateStyleSet> Create();
	static TUniquePtr<FSlateStyleSet> MSStyleInstance;
	
};
