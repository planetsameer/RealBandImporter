
#include "FRealBandAssetImporter.h"


#include "UI/FRealBandStyle.h"
#include "UI/FRealBandAssetLoader.h"
//Assets
#include "AssetRegistryModule.h"
#include "AssetViewUtils.h"
#include "AssetViewTypes.h"
//Collections
#include "CollectionManagerTypes.h"
#include "CollectionManagerModule.h"
#include "ICollectionManager.h"

// FBX
#include "Factories/FbxFactory.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxAssetImportData.h"

//GLTF
#include "GLTFImporterModule.h"
#include "../Private/GLTFImportFactory.h"
#include "../Private/GLTFImportOptions.h"

//Datasmith
#include "DatasmithTranslator.h"
#include "DatasmithImportFactory.h"
#include "DatasmithSceneFactory.h"
#include "DatasmithImportContext.h"
#include "DatasmithStaticMeshImporter.h"
#include "DatasmithScene.h"


// ContentBrowser
#include "ContentBrowserModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "IContentBrowserDataModule.h"
#include "ContentBrowserCommands.h"

//Editor
#include "LevelEditor.h"
#include "Factories/TextureFactory.h"

#include "ExternalSourceModule.h"
#include "ExternalSource.h"
#include "SourceUri.h"


//core
#include "CoreGlobals.h"
#include "CoreMinimal.h"

// Utl
#include "ImageUtils.h"
#include "UnrealEd.h"
#include "IImageWrapper.h"
#include "UnrealEdClasses.h"

#include <filesystem>

#include "InterchangeManager.h"

DEFINE_LOG_CATEGORY(LogCustom);


#define LOCTEXT_NAMESPACE "RealBandImporter"

FRealBandAssetImporter::FRealBandAssetImporter():AssetCount(0)
{

}

void FRealBandAssetImporter::Init()
{
	// Create Collections
	//CreateCollection(FName("/Engine/RealBand"));

	FString fPath = FPaths::ProjectSavedDir() / TEXT("Collections");


	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FName PathV = IContentBrowserDataModule::Get().GetSubsystem()->ConvertInternalPathToVirtual("/Engine/RealBand/Textures");

	UContentBrowserDataSubsystem* ContentBrowserData = IContentBrowserDataModule::Get().GetSubsystem();


	bool bCollection = false;
	TArray<FCollectionNameType> collectionType;
	if (FCollectionManagerModule::IsModuleAvailable())
	{
		//ICollectionManager& CollectionManager = FModuleManager::LoadModuleChecked<FCollectionManagerModule>("CollectionManager").Get();
		ICollectionManager& CollectionManager = FCollectionManagerModule::GetModule().Get();
		
		FPermissionListOwners pOwners;
		CollectionManager.GetCollectionNames(ECollectionShareType::CST_Local, pOwners);


		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray<FAssetData> AssetDataList;
		TArray<FString> APath;
		//APath.Add(TEXT("/Engine/RealBand"));
		APath.Add(TEXT("/Engine/RealBand/Textures/Prespective"));
		TArray<FName> PackageNames;
		AssetViewUtils::GetAssetsInPaths(APath, AssetDataList);


		TArray<FName> PackagePaths;
		//PackagePaths.Add(FName("/Engine/RealBand"));
		PackagePaths.Add(FName("/Engine/RealBand/Textures/Prespective"));

		bool bAdded = false;
		TArray<FAssetData> AssetsInPackages;
		IAssetRegistry* AssetRegistry = &AssetRegistryModule.Get();
		FARFilter Filter;
		Filter.bIncludeOnlyOnDiskAssets = false;
		Filter.PackagePaths = PackagePaths;
		
		TArray<FAssetData> AsstInPackages;
		AssetRegistry->GetAssets(Filter, AsstInPackages);
		TSet<FName> ObjectPathsToAddToCollection;
		TArray<FName> AssetClasses;
		for (FAssetData& ObjectAsset : AsstInPackages)
		{
			AssetClasses.Add(ObjectAsset.GetClass()->GetFName());
			ObjectPathsToAddToCollection.Add(ObjectAsset.ObjectPath);
			
		}

		


		//
		//ICollectionManager& CollectionManager = FCollectionManagerModule::GetModule().Get();
		TArray<FCollectionNameType> collectionsNames;
		CollectionManager.GetCollections(collectionsNames);
		FPermissionListOwners Owners;

		bool bResult = CollectionManager.GetAssetsInCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local, Owners);
		bResult = CollectionManager.GetAssetsInCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local, Owners);
		if (!CollectionManager.CollectionExists(FName("RealBand"), ECollectionShareType::CST_All))
		{
			bCollection = CollectionManager.CreateCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local, ECollectionStorageMode::Static);
			if (bCollection)
			{
				FPermissionListOwners listOwners;
				CollectionManager.GetCollectionNames(ECollectionShareType::CST_Local, listOwners);
				
				FString Name = listOwners[0].GetPlainNameString();
				FCollectionNameType collectObj(FName(Name), ECollectionShareType::CST_Local);
				collectionType.Add(collectObj);

				//
				FCollectionNameType collectTextObj(FName("RealBand/Textures/Prespective"), ECollectionShareType::CST_Local);
				collectionType.Add(collectTextObj);

				bool bSave = CollectionManager.SaveCollection(FName("RealBand"), ECollectionShareType::CST_Local);
				if (!bSave)
				{
					FText Ferr = CollectionManager.GetLastError();
					UE_LOG(LogCustom, Display, TEXT("%s"), *Ferr.ToString());

				}

				
			}
		}


		if (ObjectPathsToAddToCollection.Num() > 0)
		{
			bool bCollect = FCollectionManagerModule::GetModule().Get().AddToCollection(FName("RealBand"), ECollectionShareType::CST_Local, ObjectPathsToAddToCollection.Array());
			if (bCollect)
			{
				if (!FCollectionManagerModule::GetModule().Get().SaveCollection(FName("RealBand"), ECollectionShareType::CST_Local))
				{
					UE_LOG(LogCustom, Display, TEXT("Failed to Save Collection"));
				}
			}
		}


		FName outVPath;
		ContentBrowserData->TryConvertVirtualPath(FName("/All/Engine/EngineData/RealBand/Textures/Prespective"), outVPath);
		bool bCanCreate = ContentBrowserData->CanCreateFolder(PathV, nullptr);

		if (bCanCreate)
		{

			FContentBrowserItemTemporaryContext Ctx = ContentBrowserData->CreateFolder(FName("/All/EngineData/Engine/RealBand/Textures/Prespective"));
			bool bValidate = Ctx.ValidateItem(FString("Prespective"));
			if (bValidate)
			{
				Ctx.FinalizeItem(FString("Prespective"));
			}
		}
		    


	    // Initialize the Asset Viewer with the existing RealBand Assets
	    // Get the Assets from the collections
		AssetClasses.Add(FName("Texture2D"));

	    ConfigPicker.SelectionMode = ESelectionMode::Single;
	    ConfigPicker.Filter.bRecursiveClasses = false;
	    ConfigPicker.Filter.ClassNames = AssetClasses;
	    ConfigPicker.Filter.bRecursivePaths = true;
		
	    //ConfigPicker.Filter.PackagePaths= PackagePaths;
	    ConfigPicker.Collections = collectionType;
	    ConfigPicker.InitialAssetViewType = EAssetViewType::Tile;
	    ConfigPicker.bFocusSearchBoxWhenOpened = true;
	    ConfigPicker.bAllowNullSelection = true;
	    ConfigPicker.bAllowDragging = false;
	    ConfigPicker.ThumbnailLabel = EThumbnailLabel::Type::AssetName;
	   //ConfigPicker.AdditionalReferencingAssets = AssetData;
	    ConfigPicker.bAutohideSearchBar = false;
    }
	else
	{
	    UE_LOG(LogCustom, Display, TEXT("Failed to Load Collection Manager Module"));
	}


}

void FRealBandAssetImporter::GetAssetConfig(FAssetPickerConfig& oConfigPicker)
{
	oConfigPicker = ConfigPicker;
}


void FRealBandAssetImporter::CreateTexturesFromAssets(const FText& iAssetFolder)
{
    // do not want to re import the textures
	if (AssetCount > 0)
		return;

	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	//std::filesystem::path(iAssetFolder.ToString().GetCharArray());
	TArray<FString> TextureFiles;
	for (const auto& dirEntry : recursive_directory_iterator(iAssetFolder.ToString().GetCharArray().GetData()))
	{
		if (dirEntry.path().extension() == ".png") {
			if (dirEntry.path().filename().string().find("perspective") != std::string::npos)
			{
				TextureFiles.Add(FString(dirEntry.path().string().data()));
			}
		}
	}

	UTextureFactory* TexFactory = NewObject<UTextureFactory>(UTextureFactory::StaticClass());
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> ImportedTextures = AssetToolsModule.Get().ImportAssets(TextureFiles, FString("/Engine/RealBand/Textures/Prespective"),
		NewObject<UTextureFactory>(UTextureFactory::StaticClass()), false, nullptr, false);
	AssetCount = ImportedTextures.Num();
	
	UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));
}


void FRealBandAssetImporter::ImportSelectedAssets(const TArray<FName>& iAssetList, const FText & iAssetFolder, const USRPREFERENCE& iUserPreference)
{
	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	//std::filesystem::path(iAssetFolder.ToString().GetCharArray());
	TArray<FString> FbxFiles;
	TArray<FString> GlmFiles;
	TSet<FString> ObjFiles;
	TSet<FString> DiffuseFiles;
	TSet<FString> NormalFiles;
	for (FName AssetName : iAssetList)
	{
	
		//form the fbx file
		int pos = -1;
		std::wstring assetStr = AssetName.ToString().GetCharArray().GetData();
		
		bool bFind = AssetName.ToString().FindLastChar('_', pos);
		std::wstring wsubstr =  assetStr.substr(pos+1, assetStr.length());

		for (const auto& dirEntry : recursive_directory_iterator(iAssetFolder.ToString().GetCharArray().GetData()))
		{
			std::string subStr(wsubstr.begin(), wsubstr.end());
			
			if (dirEntry.path().extension() == ".fbx") {

				if (dirEntry.path().string().find(subStr) != std::string::npos)
				{
					FbxFiles.Add(FString(dirEntry.path().string().data()));
				}
			}

			if (dirEntry.path().extension() == ".glb") {
				if (dirEntry.path().filename().string().find(subStr) != std::string::npos)
				{
					GlmFiles.Add(FString(dirEntry.path().string().data()));
				}
			}

			if (dirEntry.path().extension() == ".obj") {
				if (dirEntry.path().filename().string().find(subStr) != std::string::npos)
				{
					ObjFiles.Add(FString(dirEntry.path().string().data()));
				}
			}

			if (dirEntry.path().extension() == ".png") {
				if (dirEntry.path().filename().string().find(subStr) != std::string::npos &&
					dirEntry.path().filename().string().find("diffuse") != std::string::npos)
				{
					DiffuseFiles.Add(FString(dirEntry.path().string().data()));
				}
				if (dirEntry.path().filename().string().find(subStr) != std::string::npos &&
					dirEntry.path().filename().string().find("normal") != std::string::npos)
				{
					NormalFiles.Add(FString(dirEntry.path().string().data()));
				}
			}
		}
	}

	// Import FBX
	 uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
	bool isGLM = iUserPreference.ActiveTypeBitset & Mask;
	// Import GLM
	if (isGLM)
	{
		ImportGlm(GlmFiles);
	}
	uint16 MaskF = 1 << SELECTOPTIONS::FORMAT_FBX;
	bool isFBX = iUserPreference.ActiveTypeBitset & MaskF;

	if(isFBX)
	{
		ImportFbx(FbxFiles);
	}

	

}


void FRealBandAssetImporter::ImportGlm(const TArray<FString> & iGLMFileList)
{
	TArray<FAssetData> AssetData;
	using namespace UE::DatasmithImporter;

	for (FString AssetName : iGLMFileList)
	{
		//	const FSourceUri SourceUri = FSourceUri::FromFilePath("C:\\Assets\\planeye\\planeye.glb");
		const FSourceUri SourceUri = FSourceUri::FromFilePath(AssetName);
		TSharedPtr<FExternalSource> ExternalSourcePtr = IExternalSourceModule::GetOrCreateExternalSource(SourceUri);
		FDatasmithImportContext ImportContext(ExternalSourcePtr.ToSharedRef(), true, FName(TEXT("DatasmithLibrary")),
			FText());
		const bool bSilent = true; // don't pop options window

		ImportContext.InitOptions(nullptr, FString("/Engine/RealBand"), bSilent);
		if (TSharedPtr<IDatasmithScene> LoadedScene = ExternalSourcePtr->TryLoad())
		{
			ImportContext.InitScene(LoadedScene.ToSharedRef());

		}

		ImportContext.Options->BaseOptions.AssetOptions.PackagePath = FName("/Engine/RealBand");
		ImportContext.Options->BaseOptions.bIncludeGeometry = true;
		ImportContext.Options->BaseOptions.bIncludeMaterial = true;
		ImportContext.Options->BaseOptions.bIncludeCamera = true;
		ImportContext.Options->BaseOptions.bIncludeLight = true;
		ImportContext.FeedbackContext = GWarn;
		ImportContext.Options->bUseSameOptions = true;

		//ImportContext.FilteredScene = FDatasmithSceneFactory::DuplicateScene(ImportContext.Scene.ToSharedRef());
		ImportContext.SceneName = FDatasmithUtils::SanitizeObjectName(ImportContext.Scene->GetName());
		ImportContext.ObjectFlags = RF_Public | RF_Standalone | RF_Transactional;
		ImportContext.bUserCancelled = false;
		ImportContext.AssetsContext.ReInit(FString("/Engine/RealBand"));
		bool bUserCancelled = false;
		bool bImport = DatasmithImportFactoryImpl::ImportDatasmithScene(ImportContext, bUserCancelled);
		TArray<FName> AssetClasses;
		TArray<AActor*> Actors = ImportContext.GetImportedActors();
		TArray<UObject*>  ImportedActors;
		for (AActor* Actor : Actors)
		{
			FString packagePath = Actor->GetPackage()->GetPathName();
			//	Actor->GetPackage()->SetLoadedPath(FPackagePath());
			UObject* ActorObject = dynamic_cast<UObject*>(Actor);
			UClass* AClass = Actor->GetClass();
			ImportedActors.Add(ActorObject);
			AssetData.Add(ActorObject);
			AssetClasses.Add(AClass->GetFName());
		}

		ImportedActors.Add(dynamic_cast<UObject*>(ImportContext.SceneAsset));
		AssetData.Add(dynamic_cast<UObject*>(ImportContext.SceneAsset));


		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(ImportedActors);

	}	

}




void FRealBandAssetImporter::ImportFbx(const TArray<FString> & iFbxList)
{
	// Create Collections
	bool bCollection = false;
	TArray<FCollectionNameType> collectionType;
	if (FCollectionManagerModule::IsModuleAvailable())
	{
		ICollectionManager& CollectionManager = FCollectionManagerModule::GetModule().Get();

		if (!CollectionManager.CollectionExists(FName("RealBand"), ECollectionShareType::CST_Local))
		{
			bCollection = CollectionManager.CreateCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local, ECollectionStorageMode::Static);
			FPermissionListOwners listOwners;
			CollectionManager.GetCollectionNames(ECollectionShareType::CST_Local, listOwners);

			FString Name = listOwners[0].GetPlainNameString();

			FCollectionNameType collectObj(FName(Name), ECollectionShareType::CST_Local);
			collectionType.Add(collectObj);
		}
		//else
		//{
		//	//FCollectionNameType collectObj(FName(Name), ECollectionShareType::CST_Local);
		//	FCollectionNameType collectObj(FName("/Engine/RealBand"), ECollectionShareType::CST_Local);
		//	collectionType.Add(collectObj);
		//}
	}

	// Asset Handling 
	TArray<FAssetData> AssetData;
	// FBX
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	UFbxFactory* FbxFactory = NewObject<UFbxFactory>(UFbxFactory::StaticClass());
	FbxFactory->SetDetectImportTypeOnImport(false);

	//GIsAutomationTesting = true;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	FString ImportAssetPath = TEXT("/Engine/RealBand");

	bool bSyncToBrowser = false;
	TArray<UObject*>  ImportedObjects = AssetToolsModule.Get().ImportAssets(iFbxList, ImportAssetPath, FbxFactory,
		bSyncToBrowser, nullptr, true);
	ContentBrowserModule.Get().ForceShowPluginContent(false);
	//ContentBrowserModule.Get().SyncBrowserToAssets(ImportedObjects);
	/*AssetData.Append(ImportedObjects);

	FName className = ImportedObjects[0]->GetClass()->GetFName();

	TArray<FAssetData> AssetDataList;
	TArray<FString> APath;
	APath.Add(TEXT("/Engine/RealBand"));
	TArray<FName> PackageNames;
	AssetViewUtils::GetAssetsInPaths(APath, AssetDataList);



	TArray<FName> PackagePaths;
	PackagePaths.Add(FName("/Engine/RealBand"));

	UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));*/
}


void FRealBandAssetImporter::UpdateCollections(const FName& CollectionName, const FName& PackageDir)
{
	TArray<FName> PackagePaths;
	PackagePaths.Add(PackageDir);
	PackagePaths.Add(FName("/Engine/RealBand/Textures/Prespective"));

	bool bAdded = false;
	TArray<FAssetData> AssetsInPackages;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry* AssetRegistry = &AssetRegistryModule.Get();
	FARFilter Filter;
	Filter.bIncludeOnlyOnDiskAssets = false;
	Filter.PackagePaths = PackagePaths;

	TArray<FAssetData> AsstInPackages;
	AssetRegistry->GetAssets(Filter, AsstInPackages);
	TSet<FName> ObjectPathsToAddToCollection;
	bool bCollect = false;
	for (const FAssetData& AsstData : AsstInPackages)
	{
		ObjectPathsToAddToCollection.Add(AsstData.ObjectPath);
		
	}
	if (ObjectPathsToAddToCollection.Num() > 0)
	{
		bCollect = FCollectionManagerModule::GetModule().Get().AddToCollection(CollectionName, ECollectionShareType::CST_Local, ObjectPathsToAddToCollection.Array());
		if (bCollect)
		{
			//if (!FCollectionManagerModule::GetModule().Get().SaveCollection(CollectionName, ECollectionShareType::CST_Local))
			{
			//	UE_LOG(LogManager, Display, TEXT("Failed to Save Collection"));
			}
		}
	}
	if (!FCollectionManagerModule::GetModule().Get().UpdateCollection(CollectionName, ECollectionShareType::CST_Local))
	{
		//UE_LOG(LogManager, Display, TEXT("Failed to Update Collection"));
	}
}



#undef LOCTEXT_NAMESPACE