
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

	//FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("Version 1.5")) );

	FString fPath = FPaths::ProjectSavedDir() / TEXT("Collections");


	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FName PathV = IContentBrowserDataModule::Get().GetSubsystem()->ConvertInternalPathToVirtual("/Engine/RealBand/Textures");
	
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayEngineFolder(true, true);

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
			else
			{
				FText DialogText = FText::Format(
					LOCTEXT("CollectionCreateText", "Error:Failed to Create Collection"),
					FText::FromString(TEXT("Failed to Create Collection")),
					FText::FromString(TEXT("RealBandAssetImporter.cpp"))
				);
				FMessageDialog::Open(EAppMsgType::OkCancel, DialogText);
				UE_LOG(LogCustom, Display, TEXT("Failed to Create Collection"));
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
		else
		{
			FText DialogText = FText::Format(
				LOCTEXT("PrespectiveTextureFolderCreateText", "Error:Failed to Create Prespective Folder"),
				FText::FromString(TEXT("Failed to Create Prespective Folder")),
				FText::FromString(TEXT("FRealBandAssetImporter.cpp"))
			);
			FMessageDialog::Open(EAppMsgType::OkCancel, DialogText);
			UE_LOG(LogCustom, Display, TEXT("Failed to Create Prespective Folder"));
		}

		 
		// GLB
		ContentBrowserData->TryConvertVirtualPath(FName("/All/Engine/EngineData/RealBand/GLB"), outVPath);
		bCanCreate = ContentBrowserData->CanCreateFolder(PathV, nullptr);

		if (bCanCreate)
		{

			FContentBrowserItemTemporaryContext Ctx = ContentBrowserData->CreateFolder(FName("/All/EngineData/Engine/RealBand/GLB"));
			bool bValidate = Ctx.ValidateItem(FString("GLB"));
			if (bValidate)
			{
				Ctx.FinalizeItem(FString("GLB"));
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
//	if (AssetCount > 0)
//		return;


	if (!iAssetFolder.IsEmpty() && std::filesystem::exists(std::filesystem::path(iAssetFolder.ToString().GetCharArray().GetData())))
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
		//std::filesystem::path(iAssetFolder.ToString().GetCharArray());
		TArray<FString> TextureFiles;
		for (const auto& dirEntry : recursive_directory_iterator(iAssetFolder.ToString().GetCharArray().GetData()))
		{
			if (dirEntry.path().extension() == ".png") {
				if (dirEntry.path().filename().string().find("perspective") != std::string::npos &&
					!ImportTextContext.Contains(FString(dirEntry.path().filename().wstring().data())))
				{
					TextureFiles.Add(FString(dirEntry.path().wstring().data()));
					ImportTextContext.Add(FString(dirEntry.path().filename().wstring().data()));
				}
			}
		}

		//TODO : May be we should not use ImportAssets and use texture factory to create a texture 

		UTextureFactory* TexFactory = NewObject<UTextureFactory>(UTextureFactory::StaticClass());
		TexFactory->SuppressImportOverwriteDialog(true);

		FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

		//TArray<UObject*> ImportedTextures = AssetToolsModule.Get().ImportAssets(TextureFiles, FString("/Engine/RealBand/Textures/Prespective"),
		//	NewObject<UTextureFactory>(UTextureFactory::StaticClass()), false, nullptr, false);
		TArray<UObject*> ImportedTextures = AssetToolsModule.Get().ImportAssets(TextureFiles, FString("/Engine/RealBand/Textures/Prespective"),
			TexFactory, false, nullptr, false);
		AssetCount = ImportedTextures.Num();

		

		UpdateCollections(FName("RealBand"), FName("/Engine/RealBand"));
		
	}
	else
	{
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "Error:Invalid Input Folder"),
			FText::FromString(TEXT("Invalid Input Folder")),
			FText::FromString(TEXT("RealBandImporter.cpp"))
		);
		FMessageDialog::Open(EAppMsgType::OkCancel, DialogText);
		UE_LOG(LogCustom, Display, TEXT("Error:Invalid Input Folder"));
	}
}


void FRealBandAssetImporter::ImportSelectedAssets(const TArray<FName>& iAssetList, const USRPREFERENCE& iUserPreference)
{
	if (iUserPreference.FolderPath.empty())
		return;

	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	//std::filesystem::path(iAssetFolder.ToString().GetCharArray());
	TArray<FString> FbxFiles;
	TArray<FString> GlmFiles;
	TArray<FString> ObjFiles;
	TSet<FString> DiffuseFiles;
	std::set<std::wstring> NormalFiles;
	TSet<FString> TextureFiles;
	std::set<std::wstring> filesForImport;
	

	bool isTexHigh = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::TEXHIGH);
	if (isTexHigh)
	{
		int debug = 1;
	}

	bool isTexLow = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::TEXLOW);
	if (isTexLow)
	{
		int debug = 1;
	}

	
	uint16 MaskDiff = 1 << TEXTUREOPTIONS::TDIFFUSE;
	bool isDiffuse = iUserPreference.ActiveTextypeBitset & MaskDiff;
	//bool isDiffuse = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::TDIFFUSE);
	if (isDiffuse)
	{
		Texture = Texture ^ 1 << 3;
		
	}

	uint16 MaskNormal = 1 << TEXTUREOPTIONS::TNORMAL;
	bool isNormal = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::TNORMAL);
	if (isNormal)
	{
		Texture = Texture ^ 1 << 2;
	}

	uint16 Mask2k = 1 << TEXTUREOPTIONS::TWOK;
	bool is2K = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::TWOK);
	if (is2K)
	{
		Texture = Texture ^ 1 << 6;
	}

	uint16 Mask4k = 1 << TEXTUREOPTIONS::FOURK;
	bool is4K = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::FOURK);
	if (is4K)
	{
		Texture = Texture ^ 1 << 5;
	}

	uint16 Mask8k = 1 << TEXTUREOPTIONS::EIGHTK;
	bool is8K = iUserPreference.ActiveTextypeBitset & (1 << TEXTUREOPTIONS::EIGHTK);
	if (is8K)
	{
		//Texture = Texture ^ 1 << 5;
		Texture = Texture ^ 1 << 4;
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
		if (isTexHigh)
		{
			Texture = Texture ^ 1 << 1;
		}
	}
	else
	{
		if (isTexHigh)
		{
			Texture = Texture ^ 1 << 1;
		}
	}

	uint16 MaskLow = 1 << SELECTOPTIONS::LOW;
	bool isLow = iUserPreference.ActiveTypeBitset & MaskLow;
	if (isLow)
	{
		AssetFormat = AssetFormat ^ 1 << 0;
		if (isTexLow)
		{
			Texture = Texture ^ 1 << 0;
		}
	}
	else
	{
		if (isTexLow)
		{
			Texture = Texture ^ 1 << 0;
		}
	}
	
	if (AssetFormat > 0)
	{
		char* finalRegExp = FormatExprArray[--AssetFormat];
		//UE_LOG(LogCustom, Display, TEXT("%s"), *finalRegExp);
		GetFilesForImport(iUserPreference.FolderPath, finalRegExp, filesForImport);
	}
	//for (const std::string& fileName : filesForImport)
	//{
	////	fs::path fpath{ fs::u8path(u8"abc.txt") };
	//	fs::path fpath{ fs::u8path(fileName) };
	//	std::string str = fpath.extension().string();
	//	if( fpath.extension().string() == ".fbx")
	//		FbxFiles.Add(FString(fpath.string().data()));

	//	if (fpath.extension().string() == ".glb")
	//		GlmFiles.Add(FString(fpath.string().data()));

	//	if (fpath.extension().string() == ".obj")
	//		ObjFiles.Add(FString(fpath.string().data()));
	//}
	//filesForImport.clear();

	//ImportGlm(GlmFiles);
	//ImportFbx(FbxFiles);
	//ImportFbx(ObjFiles, isOBJ);
	
	if (Texture > 0)
	{
		char* finalTextureExpr = TextureExprArray[Texture];
		GetFilesForImport(iUserPreference.FolderPath, finalTextureExpr, filesForImport);
	}
//	ImportTextures(filesForImport);
//	return;

	FbxFiles.Empty();
	for (FName AssetName : iAssetList)
	{
	
		//form the fbx file
		int pos = -1;
		std::wstring assetStr = AssetName.ToString().GetCharArray().GetData();
		
		bool bFind = AssetName.ToString().FindLastChar('_', pos);
		std::wstring wsubstr =  assetStr.substr(pos+1, assetStr.length());

		for (const std::wstring& fileName : filesForImport)
		//for (const auto& dirEntry : recursive_directory_iterator(iUserPreference.FolderPath))
		//iUserPreference
		{
			std::wstring subStr(wsubstr.begin(), wsubstr.end());
			//fs::path fpath{ fs::u8path(fileName.c_str()) };
			fs::path fpath{ fileName };
			if (fpath.extension() == ".fbx") {

				if (fpath.wstring().find(subStr) != std::string::npos)
				{
					FbxFiles.Add(FString(fpath.wstring().data()));
				}
			}
			if (fpath.extension() == ".glb") {

				if (fpath.wstring().find(subStr) != std::string::npos)
				{
					GlmFiles.Add(FString(fpath.wstring().data()));
				}
			}
			if (fpath.extension() == ".obj") {

				if (fpath.wstring().find(subStr) != std::string::npos)
				{
					ObjFiles.Add(FString(fpath.wstring().data()));
				}
			}
			if (fpath.extension() == ".png") {
				if (fpath.filename().wstring().find(subStr) != std::wstring::npos &&
					fpath.filename().wstring().find(L"diffuse") != std::wstring::npos)
				{
					//DiffuseFiles.Add(FString(fpath.string().data()));
					NormalFiles.insert(fpath.wstring());
				}
				if (fpath.filename().wstring().find(subStr) != std::wstring::npos &&
					fpath.filename().wstring().find(L"normal") != std::wstring::npos)
				{
					//NormalFiles.Add(FString(fpath.string().data()));
					NormalFiles.insert(fpath.wstring());
				}
				if (fpath.filename().wstring().find(subStr) != std::wstring::npos &&
					(
					     (fpath.filename().wstring().find(L"2k") != std::wstring::npos) ||
					     (fpath.filename().wstring().find(L"4k") != std::wstring::npos) ||
					     (fpath.filename().wstring().find(L"8k") != std::wstring::npos)
					)
					)
				{
					//NormalFiles.Add(FString(fpath.string().data()));
					NormalFiles.insert(fpath.wstring());
				}
			}
		}
	}

	// Import FBX
	// Import GLM
		ImportGlm(GlmFiles);
		ImportFbx(FbxFiles);
		ImportFbx(ObjFiles, isOBJ);
		ImportTextures(NormalFiles);
		

}


void FRealBandAssetImporter::ImportTextures(const std::set<std::wstring>& iFileList)
{
	UTextureFactory* TexFactory = NewObject<UTextureFactory>(UTextureFactory::StaticClass());

	TArray<FString> TextureFiles;
	for (const wstring & AssetName : iFileList)
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

		
		ImportContext.InitOptions(nullptr, FString("/Engine/RealBand/GLB"), bSilent);
		
		
		if (TSharedPtr<IDatasmithScene> LoadedScene = ExternalSourcePtr->TryLoad())
		{
			ImportContext.InitScene(LoadedScene.ToSharedRef());

		}

	
		ImportContext.Options->BaseOptions.AssetOptions.PackagePath = FName("/Engine/RealBand/GLB");
		ImportContext.Options->BaseOptions.bIncludeGeometry = true;
		ImportContext.Options->BaseOptions.bIncludeMaterial = true;
		ImportContext.Options->BaseOptions.bIncludeCamera = true;
		ImportContext.Options->BaseOptions.bIncludeLight = true;
		ImportContext.FeedbackContext = GWarn;
		ImportContext.Options->bUseSameOptions = true;
		const EObjectFlags NewObjectFlags = RF_Public | RF_Standalone | RF_Transactional;

		//ImportContext.FilteredScene = FDatasmithSceneFactory::DuplicateScene(ImportContext.Scene.ToSharedRef());
		ImportContext.SceneName = FDatasmithUtils::SanitizeObjectName(ImportContext.Scene->GetName());
		ImportContext.ObjectFlags = RF_Public | RF_Standalone | RF_Transactional;
		ImportContext.bUserCancelled = false;
		//ImportContext.AssetsContext.ReInit(FString("/Engine/RealBand"));
		ImportContext.AssetsContext.ReInit(FString("/Engine/RealBand/GLB"));
		
		
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
	
	FString ImportAssetPath = TEXT("/Engine/RealBand/FBX");
		TArray<UObject*>  ImportedObjects;
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	if (bObj)
	{
		ImportAssetPath = TEXT("/Engine/RealBand/OBJ");
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


void FRealBandAssetImporter::GetFilesForImport(const std::wstring& iFolderPath,const char* iFinalRegExp, std::set<std::wstring> & oFilesForImport)
{
	std::wstring wstr;
	if (iFinalRegExp)
	{
		const fs::path win_dir = iFolderPath.c_str();
	
		//for (const auto& file_path : file_list<true>(win_dir, std::regex(iFinalRegExp)))
		for (const std::filesystem::path& file_path : file_list<true>(win_dir, std::regex(iFinalRegExp)))
			oFilesForImport.insert(file_path.wstring());
	}
}


#undef LOCTEXT_NAMESPACE
