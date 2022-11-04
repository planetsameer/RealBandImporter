// Copyright RealEye, Inc. All Rights Reserved.
#include "UI/FRealBandUIManager.h"
#include "UI/FRealBandStyle.h"
#include "UI/FRealBandAssetLoader.h"

//Datasmith
#include "DatasmithTranslator.h"
#include "DatasmithImportFactory.h"
#include "DatasmithSceneFactory.h"
#include "DatasmithImportContext.h"
#include "DatasmithStaticMeshImporter.h"
#include "DatasmithScene.h"
#include "Importer/FRealBandAssetImporter.h"
//Widgets

#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Layout/ChildrenBase.h"
#include "NodeFactory.h"
#include "Brushes/SlateBoxBrush.h"

#include "ImageUtils.h"

//List of Project files
#include "Interfaces/IProjectManager.h"
#include "EditorDirectories.h"
//File picker includes
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"


// WebBrowser
#include "SWebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "AssetRegistryModule.h"
#include "AssetViewUtils.h"
#include "AssetViewTypes.h"
// FBX
#include "Factories/FbxFactory.h"
#include "Factories/FbxAnimSequenceImportData.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxAssetImportData.h"
//#include "FbxImporter.h"
//FBX

//GLTF
#include "GLTFImporterModule.h"
#include "../Private/GLTFImportFactory.h"
#include "../Private/GLTFImportOptions.h"

#include "ExternalSourceModule.h"
#include "ExternalSource.h"
#include "SourceUri.h"
//
#include "ContentBrowserModule.h"
#include "ContentBrowserItem.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"

//Editor
#include "LevelEditor.h"

//Collections
#include "CollectionManagerTypes.h"
#include "CollectionManagerModule.h"
#include "ICollectionManager.h"
//#include "AssetManagerEditorModule.h"
//core
#include "CoreGlobals.h"

//Texture
#include "Factories/TextureFactory.h"

DEFINE_LOG_CATEGORY(LogManager);


TSharedPtr<FRealBandUIManagerImpl> FRealBandUIManager::Instance;
const FName RealBandTabName = "RealBandTab";

#define LOCTEXT_NAMESPACE "RealBand"


FRealBandUIManagerImpl::FRealBandUIManagerImpl(TSharedPtr<FRealBandAssetImporter> inFRealBandAssetImporter):
	                    pFRealBandAssetImporter(inFRealBandAssetImporter)
{

}

FRealBandUIManagerImpl::~FRealBandUIManagerImpl()
{
	
	int test = 1;
}
void FRealBandUIManagerImpl::Initialize()
{
	 
	// Find all the engine installations
	TMap<FString, FString> EngineInstallations;
	FDesktopPlatformModule::Get()->EnumerateEngineInstallations(EngineInstallations);
	TSet<FString> AllFoundProjectFiles;

	for (TMap<FString, FString>::TConstIterator Iter(EngineInstallations); Iter; ++Iter)
	{
		TArray<FString> ProjectFiles;

		if (FDesktopPlatformModule::Get()->EnumerateProjectsKnownByEngine(Iter.Key(), false, ProjectFiles))
		{
			AllFoundProjectFiles.Append(MoveTemp(ProjectFiles));
		}
	}
	for (FString& ProjectFile : AllFoundProjectFiles)
	{
		FString ProjectFilename = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ProjectFile);

		if (FPaths::FileExists(ProjectFilename))
		{
			FProjectStatus ProjectStatus;
			if (IProjectManager::Get().QueryStatusForProject(ProjectFilename, ProjectStatus))
			{
				// @todo localized project name
				const FText ProjectName = FText::FromString(ProjectStatus.Name);
				Array_Resolutions.Add(MakeShareable<FString>(new FString(ProjectStatus.Name)));
				
			}
			
		}
	}

	
	//Create the MainWindow
	CreateWindow();

}



void FRealBandUIManager::Initialize(TSharedPtr<FRealBandAssetImporter> iFRealBandAssetImporter)
{
	
	if (!Instance.IsValid())
	{
		FRealBandStyle::Initialize();
		Instance = MakeShareable(new FRealBandUIManagerImpl(iFRealBandAssetImporter));
	
	}
	Instance->Initialize();
}

TSharedPtr<STextBlock> ComboBoxTitleBlock;

void FRealBandUIManagerImpl::CreateWindow()
{
	bool bIsVisible = false;
	
	if (!pDialogMainWindow )
	{
		// Get the AssetConfig from FRealBandAssetImporter
		FAssetPickerConfig ConfigPicker;
		pFRealBandAssetImporter->GetAssetConfig(ConfigPicker);

		SelectedProject = (*Array_Resolutions.begin());
		pDialogMainWindow = SNew(SWindow)
			.Title(FText::FromString("RealBand"))
			.ClientSize(FVector2D(1000, 650))
			.SupportsMaximize(false)
			.SupportsMinimize(false)
			
			[
				//SNew(SOverlay)
				SAssignNew(pOverlay, SOverlay)
				+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(pCanvas, SCanvas)
				
				+ SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(100.0f, 50.0f))
			.Position(FVector2D(30.0f, 80.0f))
			[
				SNew(SButton)
				//SNew(SButton)
				.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Text(FText::FromString("Local"))
			.OnClicked(this, &FRealBandUIManagerImpl::OnLocal)

			]
		// Asset Viewer
		+ SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(730.0f, 440.0f))
			.Position(FVector2D(150.0f, 125.0f))
			[
				SAssignNew(pFRealBandAssetLoader, FRealBandAssetLoader)
			//	.AssetPathConfig(ConfigPath)
			   .AssetPickerConfig(ConfigPicker)


			]
		/////////////////
		+ SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(100.0f, 50.0f))
			.Position(FVector2D(30.0f, 180.0f))
			[
				SAssignNew(pSettings, SButton)
				.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Text(FText::FromString("Settings"))
			.OnClicked(this, &FRealBandUIManagerImpl::LaunchSettings)
			]
		+ SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(100.0f, 50.0f))
			.Position(FVector2D(750.0f, 5.0f))
			[
				SAssignNew(pImport, SButton)
				//SNew(SButton)
				.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Text(FText::FromString("Import"))
			.OnClicked(this, &FRealBandUIManagerImpl::OnImportBtnClicked)
			]
		    + SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Position(FVector2D(50.0f, 105.0f))
			[
				//SNew(SBorder)
				//.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				//.BorderBackgroundColor(FLinearColor(0.4352F, 0.3647F, 0.8862F))
				//.Padding(3.0f)
				//[
				    SAssignNew(pSettingsBox, SBox)
				    //SNew(SBox)
				    .HAlign(HAlign_Center)
		            .VAlign(VAlign_Center)
				    .WidthOverride(400.0f)
			       [
					   SNew(SBorder)
					//   .BorderImage(FPaths::EnginePluginsDir() / TEXT("Bridge/Resources"))
				    //   .BorderBackgroundColor(FLinearColor(0.4352F, 0.3647F, 0.8862F))
				       
				       .Padding(3.0f)
				     [
			           SNew(SCanvas)
			          + SCanvas::Slot()
			          .HAlign(HAlign_Fill)
			          .VAlign(VAlign_Fill)
			          .Size(FVector2D(130.0f, 50.0f))
			          .Position(FVector2D(130.0f, 20.0f))
			          [
					     SNew(STextBlock)
					    .AutoWrapText(true)
			            .Text(FText::FromString("Download folder"))
				        .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						 //& FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText")
						 .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			         //.TextStyle(FRealBandStyle::Get(), "RealBandStyle")

			           ]
				
		             + SCanvas::Slot()
			         .HAlign(HAlign_Fill)
			         .VAlign(VAlign_Fill)
			         .Size(FVector2D(30.0f, 30.0f))
			         .Position(FVector2D(130.0f, 70.0f))
			         [
				         SNew(STextBlock)
				        .AutoWrapText(true)
			            .Text(FText::FromString("Path"))
					    .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
					
			      //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

			         ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(280.0f, 30.0f))
						 .Position(FVector2D(300.0f, 70.0f))
						 [
							 SAssignNew(pAssetFolderPath, SEditableTextBox)
						     .Text(FText::FromString("Select the Loca Path to Asset Folder"))
						 

						 //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(590.0f, 70.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						     .VAlign(VAlign_Center)
						     .OnClicked(this, &FRealBandUIManagerImpl::LaunchOpenFileDialog)
							 //.Text(FText::FromString("Select the Loca Path to Asset Folder"))


						 //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(130.0f, 90.0f))
						 .Position(FVector2D(130.0f, 130.0f))
						 [
							 SNew(STextBlock)
							 .AutoWrapText(true)
						     .Text(FText::FromString("Unreal PluginConnect"))
						     .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(130.0f, 120.0f))
						 .Position(FVector2D(130.0f, 170.0f))
						 [
							 SNew(STextBlock)
							 .AutoWrapText(true)
						 .Text(FText::FromString("Path"))
						 .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(280.0f, 30.0f))
						 .Position(FVector2D(300.0f, 170.0f))
						 [
							 SAssignNew(pAssetPath,SEditableTextBox)
							 .Text(FText::FromString("Select the Path to Asset Folder"))


						 //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(590.0f, 170.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						     .VAlign(VAlign_Center)
						     .OnClicked(this, &FRealBandUIManagerImpl::LaunchOpenFileDialog)


						 //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(130.0f, 120.0f))
						 .Position(FVector2D(130.0f, 220.0f))
						 [
							 SNew(STextBlock)
							 .AutoWrapText(true)
						 .Text(FText::FromString("Project"))
						 .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(130.0f, 40.0f))
						 .Position(FVector2D(300.0f, 220.0f))
						 [
							 
							 SNew(SHorizontalBox)
							 + SHorizontalBox::Slot()
						      .VAlign(VAlign_Fill)
						      .HAlign(HAlign_Fill)
						      .AutoWidth()
						     [
								 
								 SAssignNew(ProjectsCombo, SComboBox<TSharedPtr<FString>>)
						
								 .OptionsSource(&Array_Resolutions)
						         
						         .OnSelectionChanged_Lambda([this](TSharedPtr<FString> Value, ESelectInfo::Type)
									 {
										// SAssignNew(ComboBoxTitleBlock, STextBlock).Text(FText::FromString("ComboLabel"));
											 //	 SNew(STextBlock).Text(FText::FromString(*SelectedProject.Get()));
									//		 if (SelectedProject.Get())
											 {
												 SelectedProject = Value;
												 //ProjectsCombo->ClearSelection();
												 ProjectsCombo->SetSelectedItem(Value);
												
												 //TSharedPtr<FString> ValueS = ProjectsCombo->GetSelectedItem();
												 ProjectsCombo->RefreshOptions();
												 pTextBlock->SetText(FText::FromString(*SelectedProject.Get()));
											 }
				
									 })

		
						       
									
						         .OnGenerateWidget_Lambda([this](TSharedPtr< FString > Value)
							     {
									//	 SelectedProject = Value;
								     return SNew(STextBlock).Text(FText::FromString(*Value));
									
									 })
								.InitiallySelectedItem(SelectedProject)
								 [
									 SAssignNew(pTextBlock,STextBlock)
									 .Text(FText::FromString(*SelectedProject.Get()))
								 ]
							         ]
						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(100.0f, 50.0f))
						 .Position(FVector2D(690.0f, 290.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						 .VAlign(VAlign_Center)
						 .Text(FText::FromString("Apply"))
						 .OnClicked(this, &FRealBandUIManagerImpl::ApplySettings)


					 //    .TextStyle(FRealBandStyle::Get(), "RealBandStyle")

						 ]

					 ]// SBorder
			      ]
				//]
			    
			]


			]

			]

			];


		FSlateApplication::Get().AddWindow(pDialogMainWindow.ToSharedRef());

		pSettingsBox->SetVisibility(EVisibility::Hidden);

	}
	else
	{
	    bIsVisible = pDialogMainWindow->IsVisible();
		
    }

	pDialogMainWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
		{
		//	FRealBandUIManager::Instance.Reset();
			pDialogMainWindow.Reset();
		}));
	
//	SAssignNew(Canvas, SCanvas).FArguments(Slot().SetSize(FVector2D(700, 700)));
	//if (Windows.Num() > 0)
	{
//		FVector2D MainWindowSize = Windows[0]->GetSizeInScreen();
		float DesiredWidth = 1650;
		float DesiredHeight = 900;

		
		//if (DesiredWidth < MainWindowSize.X && DesiredHeight < MainWindowSize.Y && Canvas->GetParentWindow().IsValid())
		{
			// If Bridge is docked as a tab, the parent window will be the main window
		
			//Canvas->Resize(FVector2D(DesiredWidth, DesiredHeight));
			//LocalBrowserDock->GetParentWindow()->MoveWindowTo(FVector2D((MainWindowSize.X - DesiredWidth) - 17, MainWindowSize.Y - DesiredHeight) / 2);
		}
	}


}


void FRealBandUIManagerImpl::HandleSourceComboChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	if (Item.IsValid())
	{
		// This will call back into this widget to refresh it
		//EditorModule->SetCurrentRegistrySource(*Item);
	}
}

FReply FRealBandUIManagerImpl::OnLocal()
{
	pSettingsBox->SetVisibility(EVisibility::Collapsed);
	pFRealBandAssetLoader->SetVisibility(EVisibility::Visible);
	pImport->SetVisibility(EVisibility::Visible);
	return FReply::Handled();
}

FReply FRealBandUIManagerImpl::LaunchSettings()
{

	pSettingsBox->SetVisibility(EVisibility::Visible);
	pImport->SetVisibility(EVisibility::Collapsed);
	pFRealBandAssetLoader->SetVisibility(EVisibility::Collapsed);
	return FReply::Handled();
}


FReply FRealBandUIManagerImpl::OnImportBtnClicked()
{
	// Get the selected asset and add it to content browser
	pImport->SetVisibility(EVisibility::Visible);
	const TSharedPtr<SAssetView>& pAssetView = pFRealBandAssetLoader->GetAssetView();

	TArray<FContentBrowserItem> AssetsSelected = pAssetView->GetSelectedItems();

	TArray<FName> AssetList;
	for (FContentBrowserItem Item : AssetsSelected)
	{
		if (Item.IsValid())
		{
			FName AssetPath = Item.GetItemName();
			AssetList.Add(AssetPath);
			//PendingSyncItems.SelectedVirtualPaths.Add(Item->GetItem().GetVirtualPath());
		}
	}

	
	pFRealBandAssetImporter->ImportSelectedAssets(AssetList, pAssetPath->GetText());
	
	return FReply::Handled();
}


FReply FRealBandUIManagerImpl::ApplySettingsEx()
{
	pSettingsBox->SetVisibility(EVisibility::Collapsed);
	pImport->SetVisibility(EVisibility::Visible);

	//FString TexDir("C:\\Program Files\\Epic Games\\UE_5.0\\Engine\\Content\\Editor\\Slate\\Icons\\AssetIcons\\CameraBlockingVolume_64x.png");
	FString TexDir("C:\\Assets\\RealBand_bunddle_Asset\\RealBand_bunddle_Asset\\00001_oldCamera2\\00001_render_png_concept_oldCamera2.png");
	FString SourceImagePath = FPaths::ConvertRelativePathToFull(TexDir);
	
	UTexture2D* pTexture = nullptr;
	//pTexture = ThumbnailGenerator::GenerateThumbnailFromFile(SourceImagePath);
	pTexture = FImageUtils::ImportFileAsTexture2D(TexDir);
	pTexture->AddToRoot();
		
	

	return FReply::Handled();
}

FReply FRealBandUIManagerImpl::ApplySettings()
{
	if(pFRealBandAssetLoader->GetVisibility() == EVisibility::Visible)
		return FReply::Handled();

	pSettingsBox->SetVisibility(EVisibility::Collapsed);
	pImport->SetVisibility(EVisibility::Visible);
	
	FText AssetFolderPath = pAssetFolderPath->GetText();
	pFRealBandAssetImporter->CreateTexturesFromAssets(AssetFolderPath);

	return FReply::Handled();

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

	//pFRealBandAssetImporter->ImportGlm();
	return FReply::Handled();
	//pFRealBandAssetImporter->ImportFbx(FString(""));

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
	TArray<UObject*>  ImportedObjects = AssetToolsModule.Get().ImportAssets(CurFileToImport, ImportAssetPath, FbxFactory,
		                                                                    bSyncToBrowser,nullptr,true);
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
	//TODO: Refresh the asset view
//	pFRealBandAssetLoader->SetVisibility(EVisibility::Visible);

//	AssetViewUtils::MoveAssets(ImportedObjects, TEXT("C:\\Assets\\User"));
//	return FReply::Handled();;

	

	// GLTF
	FDatasmithSceneSource SomeCADFileSource;
	//SomeCADFileSource.SetSourceFile("C:\\Assets\\STONE_GLB.glb");
	SomeCADFileSource.SetSourceFile("C:\\Assets\\planeye\\planeye.glb");
	//TSharedPtr<IDatasmithTranslator> TranslatorForCADFiles = FDatasmithTranslatorManager::Get().SelectFirstCompatible(SomeCADFileSource);
	FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule->GetFirstActiveViewport();
	

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
	//ImportContext.Options->BaseOptions.AssetOptions.PackagePath = FName("RealBand");
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
		UClass *AClass = Actor->GetClass();
		ImportedActors.Add(ActorObject);
		AssetData.Add(ActorObject);
		AssetClasses.Add(AClass->GetFName());
	}

	ImportedActors.Add(dynamic_cast<UObject*>(ImportContext.SceneAsset));
	AssetData.Add(dynamic_cast<UObject*>(ImportContext.SceneAsset));

	// Add to the browser
	
	//ContentBrowserModule.Get().SyncBrowserToAssets(ImportedActors);
	

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
				UE_LOG(LogManager, Display, TEXT("Failed to Save Collection"));
			}
		}
	}

	if (bCollection)
	{
		FString collectString;
		TArray<FName> PackageNm;
		
		
		for (FAssetData & ObjectAsset : AssetData)
		{
			
			const FName & ObjectPath = ObjectAsset.ObjectPath;
			UPackage *PackageA = ObjectAsset.GetPackage();
			FName pName = PackageA->FileName;
			UObject* PackObject = Cast<UObject>(PackageA);
			PackageNm.Add(PackObject->GetFName());
			;
			
		}

		//const bool bCollectionSucceeded = IAssetManagerEditorModule::Get().WriteCollection(FName("RealBand"), 
		//	                                                                               ECollectionShareType::CST_Local, PackagePaths, true);
		
		//IAssetManagerEditorModule::Get().WriteCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local, PackageNames, true);
		//bool bSuccess = FAssetManagerEditorModule::GetModule().Get().WriteCollection(FName("/Engine/RealBand"), ECollectionShareType::CST_Local,PackageNames,true);
		if (bCollect)
		{
			collectString = FCollectionManagerModule::GetModule().Get().GetCollectionsStringForObject(FName(TEXT("/Engine/RealBand/XOM_FBX")), ECollectionShareType::CST_Local);
		}

		//const bool bCollectionSucceeded = FCollectionManagerModule::GetModule().Get().WriteCollection(FirstAssetName, ShareType, PackageNameSet, true);
		/*if (bCollectionSucceeded)
		{
			CollectionManager.ReparentCollection(FirstAssetName, ShareType, ParentName, ShareType);
		}

		TArray<FCollectionNameType> CollectionNamesAndTypes;
		CollectionManager.GetCollectionsContainingObject(FName("/Engine"), CollectionNamesAndTypes);
		FPermissionListOwners ownersList;
		CollectionManager.GetCollectionNames(ECollectionShareType::CST_All, ownersList);*/
	}
	

	//ContentBrowserModule.Get().SyncBrowserToAssets(ImportedActors);

	return FReply::Handled();
	
	
	ConfigPath.OnPathSelected = FOnPathSelected::CreateSP(this, &FRealBandUIManagerImpl::HandlePathSelected);
	ConfigPath.SetPathsDelegates.Add(&SetPathsDelegate);
	//AssetPicker->GetAssetView()->SetSourcesData(DefaultSourcesData);
	//TSharedPtr<FRealBandAssetLoader> pFRealBandAssetLoader;
	
	
	FAssetPickerConfig ConfigPicker;
	ConfigPicker.SelectionMode = ESelectionMode::Single;
	ConfigPicker.Filter.bRecursiveClasses = true;
	//ConfigPicker.Filter.ClassNames = AssetClasses;
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
	TArray<FString> selectedPath;
	selectedPath.Add(sourceDir);
	
	// Call the Asset Viewer
	pCanvas->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Size(FVector2D(730.0f, 440.0f))
		.Position(FVector2D(150.0f, 125.0f))
		[
			SAssignNew(pFRealBandAssetLoader, FRealBandAssetLoader)
			.AssetPathConfig(ConfigPath)
		    .AssetPickerConfig(ConfigPicker)
		    
		    
		];
	
	return FReply::Handled();
}


void FRealBandUIManagerImpl::UpdateCollections(const FName & CollectionName,const FName & PackageDir)
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
				UE_LOG(LogManager, Display, TEXT("Failed to Save Collection"));
			}
		}
	}
	if (!FCollectionManagerModule::GetModule().Get().UpdateCollection(CollectionName, ECollectionShareType::CST_Local))
	{
		UE_LOG(LogManager, Display, TEXT("Failed to Update Collection"));
	}
}

void  FRealBandUIManagerImpl::HandlePathSelected(const FString& NewPath)
{
	TArray<FString> NewPaths;
	NewPaths.Add("C:\\Program Files\\Epic Games\\UE_5.0\\Engine\\Content\\Functions\\Engine_MaterialFunctions01\\ImageAdjustment");
	SetPathsDelegate.Execute(NewPaths);
}

FReply FRealBandUIManagerImpl::LaunchOpenFileDialog()
{

	
	//		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (pDialogMainWindow)
			{
				//Opening the file picker!
				uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
				FString FolderPath;
				FString DefaultPath("C:\\Temp");
				// set a default path
				FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_EXPORT, DefaultPath);
				DesktopPlatform->OpenDirectoryDialog(pDialogMainWindow->GetNativeWindow()->GetOSWindowHandle(),
					                                 DefaultPath,
					                                 DefaultPath, 
					                                 FolderPath);
				FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_EXPORT, FolderPath);
				if (pAssetPath)
				{
					pAssetPath->SetText(FText::FromString(FolderPath));
					pAssetFolderPath->SetText(FText::FromString(FolderPath));
				}

				
	//			DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
			}
	//	}
	//}

	return FReply::Handled();
}


TSharedRef<SDockTab> FRealBandUIManagerImpl::CreatRealBandTab(const FSpawnTabArgs& Args)
{
	FWebBrowserInitSettings browserInitSettings = FWebBrowserInitSettings();
	IWebBrowserModule::Get().CustomInitialize(browserInitSettings);
	TSharedPtr<SWebBrowser> PluginWebBrowser;
//	WindowSettings.InitialURL = FinalUrl;
	WindowSettings.BrowserFrameRate = 60;

	if (IWebBrowserModule::IsAvailable() && IWebBrowserModule::Get().IsWebModuleAvailable())
	{
		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
		Browser = WebBrowserSingleton->CreateBrowserWindow(WindowSettings);
		PluginWebBrowser = SAssignNew(WebBrowserWidget, SWebBrowser, Browser)
			.ShowAddressBar(false)
			.ShowControls(false);
	}

	//
	return SAssignNew(LocalBrowserDock, SDockTab);
}


TStrongObjectPtr<UDatasmithGLTFImportOptions>& FRealBandUIManagerImpl::GetOrCreateGLTFImportOptions()
{
	if (!ImportOptions.IsValid() && IsInGameThread())
	{
	//	ImportOptions = Datasmith::MakeOptions<UDatasmithGLTFImportOptions>();
	}
	return ImportOptions;
}

#undef LOCTEXT_NAMESPACE