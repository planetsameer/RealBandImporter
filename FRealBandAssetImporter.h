#pragma once

#include "CoreMinimal.h"

#include "IContentBrowserSingleton.h"
#include "RealBandCommon.h"
class FAssetViewItem;

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);



class FRealBandAssetImporter : FNoncopyable
{
private:
	FAssetPickerConfig ConfigPicker;
	int AssetCount;
	void UpdateCollections(const FName& CollectionName, const FName& PackageDir);
public:
	FRealBandAssetImporter();
	void GetAssetConfig(FAssetPickerConfig& oConfigPicker);
	void Init();
	void ImportFbx(const TArray<FString>&);
	void ImportGlm(const TArray<FString>&);
	void CreateTexturesFromAssets(const FText &);
	void GetAssetsCount(int oAssetCount) { oAssetCount = AssetCount; }
	void ImportSelectedAssets(const TArray<FName> &, const FText& iAssetFolder, const USRPREFERENCE & );
	//void ImportSelectedAssets(const TArray<TSharedPtr<FAssetViewItem&>> & iAssetList) const;
};