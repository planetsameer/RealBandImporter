#pragma once

#include "CoreMinimal.h"

#include "IContentBrowserSingleton.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);

class FRealBandAssetImporter : FNoncopyable
{
private:
	FAssetPickerConfig ConfigPicker;
	
public:
	void GetAssetConfig(FAssetPickerConfig& oConfigPicker);
	void Init();
};