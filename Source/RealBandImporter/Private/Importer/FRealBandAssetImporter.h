#pragma once

#include "CoreMinimal.h"

#include "IContentBrowserSingleton.h"
class FAssetViewItem;

DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);



class FRealBandAssetImporter : FNoncopyable
{
private:
	FAssetPickerConfig ConfigPicker;
	void UpdateCollections(const FName& CollectionName, const FName& PackageDir);
public:
	void GetAssetConfig(FAssetPickerConfig& oConfigPicker);
	void Init();
	void ImportFbx(const TArray<FString>&);
	void ImportGlm(const TArray<FString>&);
	void CreateTexturesFromAssets(const FText &);
	void ImportSelectedAssets(const TArray<FName> &, const FText& iAssetFolder);
	//void ImportSelectedAssets(const TArray<TSharedPtr<FAssetViewItem&>> & iAssetList) const;
};