
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
#include <regex>
#include <type_traits>

#include "InterchangeManager.h"

DEFINE_LOG_CATEGORY(LogCustom);


#define LOCTEXT_NAMESPACE "RealBandImporter"


namespace fs = std::filesystem;

template < bool RECURSIVE > std::vector<fs::path> file_list(fs::path dir, std::regex ext_pattern)
{
	std::vector<fs::path> result;

	using iterator = typename std::conditional< RECURSIVE,
		typename fs::recursive_directory_iterator, typename fs::directory_iterator >::type;

	//   typename fs::recursive_directory_iterator iterator =  std::conditional< RECURSIVE,
	//       fs::recursive_directory_iterator, fs::directory_iterator >::type;

	typename fs::recursive_directory_iterator  end;
	for (typename fs::recursive_directory_iterator iter{ dir }; iter != end; ++iter)
	{
		int pos = -1;
		const std::string fname = iter->path().filename().string();
		//std::string ext = iter->path().extension().string();
//         fname += iter->path().extension().string();
	   // bool bFind = fname.find_last_of("\\\\", pos);
	   // std::string substr = fname.substr(pos + 1, fname.length());


		if (std::regex_match(fname, ext_pattern))
			result.push_back(*iter);

		/* const std::string extension = iter->path().extension().string();
		 if (fs::is_regular_file(*iter) && std::regex_match(extension, ext_pattern))
			 result.push_back(*iter);*/


	}

	return result;
}


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
	TArray<FString> ObjFiles;
	TSet<FString> DiffuseFiles;
	TSet<FString> NormalFiles;
	TSet<FString> TextureFiles;
	
	

	uint16 MaskDiff = 1 << SELECTOPTIONS::DIFFUSE;
	//bool isDiffuse = iUserPreference.ActiveTypeBitset & MaskDiff;
	bool isDiffuse = iUserPreference.ActiveTypeBitset & (1 << SELECTOPTIONS::DIFFUSE);
	if (isDiffuse)
	{
		Texture = Texture ^ 1 << 4;
	}

	uint16 MaskNormal = 1 << SELECTOPTIONS::NORMAL;
	bool isNormal = iUserPreference.ActiveTypeBitset & (1 << SELECTOPTIONS::NORMAL);
	if (isNormal)
	{
		Texture = Texture ^ 1 << 3;
	}

	uint16 Mask2k = 1 << SELECTOPTIONS::TWO_K;
	bool is2K = iUserPreference.ActiveTypeBitset & (1 << SELECTOPTIONS::TWO_K);
	if (is2K)
	{
		Texture = Texture ^ 1 << 7;
	}

	uint16 Mask4k = 1 << SELECTOPTIONS::FOUR_K;
	bool is4K = iUserPreference.ActiveTypeBitset & (1 << SELECTOPTIONS::FOUR_K);
	if (is4K)
	{
		Texture = Texture ^ 1 << 6;
	}

	uint16 Mask8k = 1 << SELECTOPTIONS::EIGHT_K;
	bool is8K = iUserPreference.ActiveTypeBitset & (1 << SELECTOPTIONS::EIGHT_K);
	if (is8K)
	{
		Texture = Texture ^ 1 << 5;
	}

	uint16 MaskF = 1 << SELECTOPTIONS::FORMAT_FBX;
	bool isFBX = iUserPreference.ActiveTypeBitset & MaskF;
	if (isFBX)
		AssetFormat = AssetFormat ^ 1 << 4;


	uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
	bool isGLM = iUserPreference.ActiveTypeBitset & Mask;
	if (isGLM)
		AssetFormat = AssetFormat ^ 1 << 3;

	uint16 MaskO = 1 << SELECTOPTIONS::FORMAT_OBJ;
	bool isOBJ = iUserPreference.ActiveTypeBitset & MaskO;
	if (isOBJ)
		AssetFormat = AssetFormat ^ 1 << 2;

	uint16 MaskHigh = 1 << SELECTOPTIONS::HIGH;
	bool isHigh = iUserPreference.ActiveTypeBitset & MaskHigh;
	if (isHigh)
	{
		AssetFormat = AssetFormat ^ 1 << 1;
		Texture = Texture ^ 1 << 1;
	}

	uint16 MaskLow = 1 << SELECTOPTIONS::LOW;
	bool isLow = iUserPreference.ActiveTypeBitset & MaskLow;
	if (isLow)
	{
		AssetFormat = AssetFormat ^ 1 << 0;
		Texture = Texture ^ 1 << 0;
	}
	
	char *finalRegExp = FormatExprArray[AssetFormat-1];
	std::set<std::string> filesForImport;
	GetFilesForImport(iUserPreference.FolderPath,finalRegExp, filesForImport);
	for (const std::string& fileName : filesForImport)
	{
	//	fs::path fpath{ fs::u8path(u8"abc.txt") };
		fs::path fpath{ fs::u8path(fileName) };
		std::string str = fpath.extension().string();
		if( fpath.extension().string() == ".fbx")
			FbxFiles.Add(FString(fpath.string().data()));

		if (fpath.extension().string() == ".glb")
			GlmFiles.Add(FString(fpath.string().data()));

		if (fpath.extension().string() == ".obj")
			ObjFiles.Add(FString(fpath.string().data()));
	}
	filesForImport.clear();

	ImportGlm(GlmFiles);
	ImportFbx(FbxFiles);
	ImportFbx(ObjFiles, isOBJ);
	Texture = Texture - 1;
//	char *testExpr = TextureExprArray[30];
//	GetFilesForImport(iUserPreference.FolderPath, testExpr, filesForImport);
//	testExpr = TextureExprArray[32];
//	GetFilesForImport(iUserPreference.FolderPath, testExpr, filesForImport);
	finalRegExp = TextureExprArray[Texture];
	GetFilesForImport(iUserPreference.FolderPath, finalRegExp, filesForImport);
	ImportTextures(filesForImport);
	return;
	/*for (std::string fileName : filesForImport)
	{
		fs::path fpath{ fs::u8path(fileName) };
		std::string str = fpath.extension().string();
		if (fpath.extension().string() == ".png")
		{
			ImportTexture(fpath.extension().string());
		}
	}*/

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
	// Import GLM
	if (isGLM)
	{
		ImportGlm(GlmFiles);
	}

	if(isFBX)
	{
		ImportFbx(FbxFiles);
	}

	if (isOBJ)
	{
		ImportFbx(ObjFiles, isOBJ);
	}

	if (isDiffuse)
	{
		for (FString DiffFile : DiffuseFiles)
		{

		}
		
	}
}


void FRealBandAssetImporter::ImportTextures(const std::set<std::string>& iFileList)
{
	UTextureFactory* TexFactory = NewObject<UTextureFactory>(UTextureFactory::StaticClass());

	TArray<FString> TextureFiles;
	for (const string & AssetName : iFileList)
	{
		TextureFiles.Add(AssetName.c_str());
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> ImportedObjects = AssetToolsModule.Get().ImportAssets(TextureFiles, FString("/Engine/RealBand/Textures"),
		NewObject<UTextureFactory>(UTextureFactory::StaticClass()), false, nullptr, false);
	
	int ObjCount = ImportedObjects.Num();

	UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));

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




void FRealBandAssetImporter::ImportFbx(const TArray<FString> & iFbxList, bool bObj)
{
	if (iFbxList.IsEmpty())
		return;
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
	// OBJ
	bool bSyncToBrowser = false;
	FString ImportAssetPath = TEXT("/Engine/RealBand");
	TArray<UObject*>  ImportedObjects;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	if (bObj)
	{
		ImportedObjects = AssetToolsModule.Get().ImportAssets(iFbxList, ImportAssetPath, nullptr,
			bSyncToBrowser, nullptr, true);
	}
	
	else
	{
		// FBX
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		UFbxFactory* FbxFactory = NewObject<UFbxFactory>(UFbxFactory::StaticClass());
		FbxFactory->SetDetectImportTypeOnImport(false);
		ImportedObjects = AssetToolsModule.Get().ImportAssets(iFbxList, ImportAssetPath, FbxFactory,
			bSyncToBrowser, nullptr, true);

		ContentBrowserModule.Get().ForceShowPluginContent(false);
	}
	
}


void FRealBandAssetImporter::ApplyUserPreferences(const USRPREFERENCE& iUserPreference)
{

	// Prepare Filter for Formats
	//^ [0 - 9] + _mesh_fbx_low_\\w + \\.(? : fbx | glb | obj)
	std::string regExprStrFormatTypes("^ [0 - 9] + _mesh_[");
	//check for preferences glb,fbx,obj
	uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
	bool isGLM = iUserPreference.ActiveTypeBitset & Mask;
	uint16 MaskF = 1 << SELECTOPTIONS::FORMAT_FBX;
	bool isFBX = iUserPreference.ActiveTypeBitset & MaskF;
	uint16 MaskO = 1 << SELECTOPTIONS::FORMAT_OBJ;
	bool isOBJ = iUserPreference.ActiveTypeBitset & MaskO;
	uint16 MaskHigh = 1 << SELECTOPTIONS::HIGH;
	bool isHigh = iUserPreference.ActiveTypeBitset & MaskHigh;
	uint16 MaskLow = 1 << SELECTOPTIONS::LOW;
	bool isLow = iUserPreference.ActiveTypeBitset & MaskLow;

	if (isGLM)
	{
		regExprStrFormatTypes.append("glb");
	}
	if (isFBX)
	{
		regExprStrFormatTypes.append(",fbx");
	}
	if (isOBJ)
	{
		regExprStrFormatTypes.append(",obj");
	}
	regExprStrFormatTypes.append("]_");
	if (isHigh)
	{
		regExprStrFormatTypes.append("[High");
	}
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


void FRealBandAssetImporter::GetFilesForImport(const std::string& iFolderPath,const char* iFinalRegExp, std::set<std::string> & oFilesForImport)
{
	if (iFinalRegExp)
	{
		const fs::path win_dir = iFolderPath.c_str();
		//for (const auto& file_path : file_list<true>(win_dir, std::regex(iFinalRegExp)))
		for (const std::filesystem::path& file_path : file_list<true>(win_dir, std::regex(iFinalRegExp)))
			oFilesForImport.insert(file_path.string());
	}
}


#undef LOCTEXT_NAMESPACE