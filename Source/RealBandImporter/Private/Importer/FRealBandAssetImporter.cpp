
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


DEFINE_LOG_CATEGORY(LogCustom);


#define LOCTEXT_NAMESPACE "RealBandImporter"

void FRealBandAssetImporter::Init()
{
	// Create Collections
	//CreateCollection(FName("/Engine/RealBand"));

	FString fPath = FPaths::ProjectSavedDir() / TEXT("Collections");

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
		APath.Add(TEXT("/Engine/RealBand"));
		TArray<FName> PackageNames;
		AssetViewUtils::GetAssetsInPaths(APath, AssetDataList);


		TArray<FName> PackagePaths;
		PackagePaths.Add(FName("/Engine/RealBand"));

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
	    // Initialize the Asset Viewer with the existing RealBand Assets
	    // Get the Assets from the collections

	    ConfigPicker.SelectionMode = ESelectionMode::Single;
	    ConfigPicker.Filter.bRecursiveClasses = true;
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
	// Extract glm texture files 

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

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> ImportedSheets = AssetToolsModule.Get().ImportAssets(TextureFiles, FString("/Engine/RealBand"),
		NewObject<UTextureFactory>(UTextureFactory::StaticClass()), false, nullptr, false);

	UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));
}


void FRealBandAssetImporter::ImportSelectedAssets(const TArray<FName>& iAssetList, const FText & iAssetFolder)
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
	
	{
		ImportFbx(FbxFiles);
	}

	// Import GLM

	{
		ImportGlm(GlmFiles);
	}

}


void FRealBandAssetImporter::ImportGlm(const TArray<FString> & iGLMFileList)
{
	FDatasmithSceneSource SomeCADFileSource;
	//SomeCADFileSource.SetSourceFile("C:\\Assets\\STONE_GLB.glb");
	SomeCADFileSource.SetSourceFile("C:\\Assets\\planeye\\planeye.glb");
	//TSharedPtr<IDatasmithTranslator> TranslatorForCADFiles = FDatasmithTranslatorManager::Get().SelectFirstCompatible(SomeCADFileSource);
	FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule->GetFirstActiveViewport();
	TArray<FAssetData> AssetData;

	using namespace UE::DatasmithImporter;
	//const FSourceUri SourceUri = FSourceUri::FromFilePath("C:\\Assets\\STONE_GLB.glb");
	const FSourceUri SourceUri = FSourceUri::FromFilePath("C:\\Assets\\planeye\\planeye.glb");
	TSharedPtr<FExternalSource> ExternalSourcePtr = IExternalSourceModule::GetOrCreateExternalSource(SourceUri);
	const TSharedPtr<IDatasmithTranslator>& TranslatorPtr = ExternalSourcePtr->GetAssetTranslator();
	FDatasmithTranslatorCapabilities OutCapabilities;
	TranslatorPtr->Initialize(OutCapabilities);
	TranslatorPtr->SetSource(SomeCADFileSource);
	FDatasmithImportContext ImportContext(ExternalSourcePtr.ToSharedRef(), true, FName(TEXT("DatasmithLibrary")),
		FText());
	const bool bSilent = false; // don't pop options window

	//ImportContext.InitOptions(nullptr, TOptional<FString>(), bSilent);
	//ImportContext.InitOptions(nullptr, FString("C:\\Assets\\STONE_GLB.glb"), bSilent);
	//ImportContext.InitOptions(nullptr, FString("C:\\Assets\\planeye\\planeye.glb"), bSilent);
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

	//ImportContext.FilteredScene = FDatasmithSceneFactory::DuplicateScene(ImportContext.Scene.ToSharedRef());
	ImportContext.SceneName = FDatasmithUtils::SanitizeObjectName(ImportContext.Scene->GetName());
	ImportContext.ObjectFlags = RF_Public | RF_Standalone | RF_Transactional;
	ImportContext.bUserCancelled = false;
	ImportContext.AssetsContext.ReInit(FString("/Engine/RealBand"));
	//ImportContext.AssetsContext.ReInit(FString("RealBand"));

	// We do not want to import into any View 
	//ImportContext.ActorsContext.ImportWorld = GWorld; // Make sure actors are imported(into the current world)


	// ImportDatasmithscene already does this
	//FDatasmithImporter::ImportTextures(ImportContext);
	//FDatasmithImporter::ImportMaterials(ImportContext);

	//FDatasmithImporter::ImportStaticMeshes(ImportContext);
	//FDatasmithStaticMeshImporter::PreBuildStaticMeshes(ImportContext);


//	TSharedRef<IDatasmithScene> LoadedScene = FDatasmithSceneFactory::CreateScene(L"STONE_GLB");
//	ImportContext.InitScene(LoadedScene);
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


	// Lets create Asset out of the texture thumbnail and add it to viewer
	// Texture Creation
	

	FString TexDir("C:\\Assets\\RealBand_bunddle_Asset\\RealBand_bunddle_Asset\\00001_oldCamera2\\00001_render_png_perspective_oldCamera2.png");
	//FString SourceImagePath = FPaths::ConvertRelativePathToFull(TexDir);

	//UTexture2D* pTexture = nullptr;
	//pTexture = ThumbnailGenerator::GenerateThumbnailFromFile(SourceImagePath);
	
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	TArray<FString> TextureFileNames;
	TextureFileNames.Add(TexDir);
	TArray<UObject*> ImportedSheets = AssetToolsModule.Get().ImportAssets(TextureFileNames, FString("/Engine/RealBand"),
		                                                                  NewObject<UTextureFactory>(UTextureFactory::StaticClass()),false,nullptr,false);
	
	
	FString sheetPath = ImportedSheets[0]->GetPackage()->GetPathName();
	UClass *pClass = ImportedSheets[0]->GetPackage()->GetClass();

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	//ContentBrowserModule.Get().SyncBrowserToAssets(ImportedActors);
	TArray<FName> PackagePaths;
	PackagePaths.Add(FName("/Engine/RealBand"));

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	bool bAdded = false;
	TArray<FAssetData> AssetsInPackages;
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
		bCollect = FCollectionManagerModule::GetModule().Get().AddToCollection(FName("RealBand"), ECollectionShareType::CST_Local, ObjectPathsToAddToCollection.Array());
		if (bCollect)
		{
			if (!FCollectionManagerModule::GetModule().Get().SaveCollection(FName("RealBand"), ECollectionShareType::CST_Local))
			{
				//UE_LOG(LogManager, Display, TEXT("Failed to Save Collection"));
			}
		}
	}

}




void FRealBandAssetImporter::ImportFbx(const TArray<FString> & iFbxList)
{
	FString sourceDir("RealBand");
	FPathPickerConfig ConfigPath;
	ConfigPath.DefaultPath = sourceDir;



	// Create Collections
	bool bCollection = false;
	TArray<FCollectionNameType> collectionType;
	if (FCollectionManagerModule::IsModuleAvailable())
	{
		ICollectionManager& CollectionManager = FCollectionManagerModule::GetModule().Get();

		//	if (!CollectionManager.CollectionExists(FName("RealBand"), ECollectionShareType::CST_Local))
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
	//FString FbxFileToImport("C:\\Assets\\XOM_FBX.fbx");
	FString FbxFileToImport("C:\\Assets\\cadnvas\\basketballcourt.FBX");
	TArray<FString> CurFileToImport;
	CurFileToImport.Add(FbxFileToImport);
	//FString ImportAssetPath = TEXT("/Game");
	//FString ImportAssetPath = TEXT("/Engine/Materials");
	FString ImportAssetPath = TEXT("/Engine/RealBand");

	bool bSyncToBrowser = false;
	TArray<UObject*>  ImportedObjects = AssetToolsModule.Get().ImportAssets(iFbxList, ImportAssetPath, FbxFactory,
		bSyncToBrowser, nullptr, true);
	ContentBrowserModule.Get().ForceShowPluginContent(false);
	//ContentBrowserModule.Get().SyncBrowserToAssets(ImportedObjects);
	AssetData.Append(ImportedObjects);


	TArray<FAssetData> AssetDataList;
	TArray<FString> APath;
	APath.Add(TEXT("/Engine/RealBand"));
	TArray<FName> PackageNames;
	AssetViewUtils::GetAssetsInPaths(APath, AssetDataList);



	TArray<FName> PackagePaths;
	PackagePaths.Add(FName("/Engine/RealBand"));

	UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));
}


void FRealBandAssetImporter::UpdateCollections(const FName& CollectionName, const FName& PackageDir)
{
	TArray<FName> PackagePaths;
	PackagePaths.Add(PackageDir);

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
			if (!FCollectionManagerModule::GetModule().Get().SaveCollection(CollectionName, ECollectionShareType::CST_Local))
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


//void FRealBandAssetImporter::ImportSelectedAssets(const TArray<TSharedPtr<FAssetViewItem &>>  & iAssetList) const
//{
//	// start the actual import 
//	for (const TSharedPtr<FAssetViewItem &>  & Item : iAssetList)
//	{
//		// Update the custom column data
//		FAssetData ItemAssetData;
//		if (Item->GetItem().Legacy_TryGetAssetData(ItemAssetData))
//		{
//
//		}
//
//	}
//
//}

#undef LOCTEXT_NAMESPACE