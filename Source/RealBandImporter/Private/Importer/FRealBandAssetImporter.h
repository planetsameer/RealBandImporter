//Copyright(C) 2022 VAXPORT all rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "IContentBrowserSingleton.h"
#include "RealBandCommon.h"
#include "Delegates/DelegateCombinations.h"
#include <set>
class FAssetViewItem;

DECLARE_LOG_CATEGORY_EXTERN(LogRealBand, Log, All);



class FRealBandAssetImporter : FNoncopyable
{
private:
	FAssetPickerConfig ConfigPicker;
	int AssetCount;
	void UpdateCollections(const FName& CollectionName, const FName& PackageDir);
	void GetFilesForImport(const std::wstring & iFolderPath, const char* iFinalRegExp, std::set<std::wstring>& oFilesForImport);
	void ImportTextures(const std::set<std::wstring> &);
	TSet<FString> ImportTextContext;
public:
	FRealBandAssetImporter();
	void GetAssetConfig(FAssetPickerConfig& oConfigPicker);
	bool Init();
	void ImportFbx(const TArray<FString>&, bool bObj = false);
	void ImportGlm(const TArray<FString>&);
	void CreateTexturesFromAssets(const FText &);
	void GetAssetsCount(int oAssetCount) { oAssetCount = AssetCount; }
	void ImportSelectedAssets(const TArray<FName> &, const USRPREFERENCE & );
	void ApplyUserPreferences(const USRPREFERENCE &);
	void UpdateAssetFolder(FString);

	//void ImportSelectedAssets(const TArray<TSharedPtr<FAssetViewItem&>> & iAssetList) const;
	FStringDelegate DirChangeDelegate;
};