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
#include "Widgets/Layout/SGridPanel.h"
#include "NodeFactory.h"
#include "Brushes/SlateBoxBrush.h"

#include "ImageUtils.h"
#include "Brushes/SlateImageBrush.h"
#include "Styling/SlateStyle.h"
#include "Styling/StyleColors.h"

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
#include "SAssetView.h"

//Editor
#include "LevelEditor.h"
#include "PropertyEditor/Private/SDetailCategoryTableRow.h"

//Collections
#include "CollectionManagerTypes.h"
#include "CollectionManagerModule.h"
#include "ICollectionManager.h"
//#include "AssetManagerEditorModule.h"
//core
#include "CoreGlobals.h"

//Texture
#include "Factories/TextureFactory.h"

#include "Styling/SlateStyleMacros.h"

//uint16 FRealBandUIManagerImpl::ActiveTypeBitset = TNumericLimits<uint16>::Max();


DEFINE_LOG_CATEGORY(LogManager);


TSharedPtr<FRealBandUIManagerImpl> FRealBandUIManager::Instance;
const FName RealBandTabName = "RealBandTab";

#define LOCTEXT_NAMESPACE "RealBand"


FRealBandUIManagerImpl::FRealBandUIManagerImpl(TSharedPtr<FRealBandAssetImporter> inFRealBandAssetImporter):
	                    pFRealBandAssetImporter(inFRealBandAssetImporter)
{
	ObjUserPreference.ActiveTypeBitset = TNumericLimits<uint16>::Max();
}

FRealBandUIManagerImpl::~FRealBandUIManagerImpl()
{
	//FRealBandUIManager::Instance.Reset();
	//pFRealBandAssetLoader.Reset();
	pOverlay.Reset();
	pCanvas.Reset();
	//pFRealBandAssetLoader.Reset();
	ObjUserPreference.ActiveTypeBitset = TNumericLimits<uint16>::Max();
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
			.SizingRule(ESizingRule::UserSized)
			[
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
			   .AssetPickerConfig(ConfigPicker)
			]
		/*+SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(730.0f, 440.0f))
			.Position(FVector2D(150.0f, 125.0f))
			[
				SAssignNew(pGridPanel, SGridPanel)
			]*/
		///////////////////
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
				.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Text(FText::FromString("Import"))
			.OnClicked(this, &FRealBandUIManagerImpl::OnImportBtnClicked)
			]
		    + SCanvas::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Size(FVector2D(850.0f, 500.0f))
			.Position(FVector2D(140.0f, 95.0f))
			[
				SAssignNew(pSettingsWindow, SWindow)
				.Title(FText::FromString("Settings"))
				//.ClientSize(FVector2D(300, 300))
				.Style(&FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window"))
				
				//.UseOSWindowBorder(true)
				.SupportsMaximize(false)
				.SupportsMinimize(false)
				[
			    SAssignNew(pSettingsBox, SBox)
			    //.HAlign(HAlign_Center)
	            //.VAlign(VAlign_Center)
			    .WidthOverride(400.0f)
				.HeightOverride(600.0f)
			     [ 
				
					 //  SNew(SBorder)
					//   .BorderImage(FPaths::EnginePluginsDir() / TEXT("Bridge/Resources"))
				    //   .BorderBackgroundColor(FLinearColor(0.4352F, 0.3647F, 0.8862F))
				       
				     //  .Padding(3.0f)
				     //[
					 SAssignNew(pSettingsCanvasBox, SCanvas)
			          // SNew(SCanvas)
			          + SCanvas::Slot()
			          .HAlign(HAlign_Fill)
			          .VAlign(VAlign_Fill)
			          .Size(FVector2D(530.0f, 450.0f))
			          .Position(FVector2D(80.0f, 30.0f))
			          [
					     SNew(STextBlock)
					    .AutoWrapText(false)
			            .Text(FText::FromString("Download folder Settings"))
				        .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
				        .MinDesiredWidth(350.0f)
						 //& FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText")
						 .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("Text.Large"))
			         //.TextStyle(FRealBandStyle::Get(), "RealBandStyle")

			           ]
				
		             + SCanvas::Slot()
			         .HAlign(HAlign_Fill)
			         .VAlign(VAlign_Fill)
			         .Size(FVector2D(30.0f, 30.0f))
			         .Position(FVector2D(130.0f, 80.0f))
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
						 .Position(FVector2D(300.0f, 80.0f))
						 [
							 SAssignNew(pAssetFolderPath, SEditableTextBox)
						     .Text(FText::FromString("Select the Local Path to Asset Folder"))
						     .OnTextChanged(this,&FRealBandUIManagerImpl::OnFolderPathChanged)

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(590.0f, 80.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						     .VAlign(VAlign_Center)
						     .OnClicked(this, &FRealBandUIManagerImpl::LaunchOpenFileDialog)

						 ]

					 //  Download folder settings Box
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(130.0f, 120.0f))
						 [
							 SNew(SBox)
							 [
								 SNew(SHorizontalBox)
								 + SHorizontalBox::Slot()
						          .AutoWidth()
						          .Padding(2, 0, 0, 0)
						         [
									 SNew(STextBlock)
									 .Text(FText::FromString("Format"))
						             .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						             .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
								 ]

								 + SHorizontalBox::Slot()
						           //.AutoWidth()
								//	 .Padding(120, 0, 80, 0)
						           .Padding(120, 0, 50, 0)
								   
						            [
										GetSelectedOptionToggle(SELECTOPTIONS::FORMAT_GLM,  LOCTEXT("glb", "glb"))
						            ]
					                + SHorizontalBox::Slot()
									.Padding(2, 0, 60, 0)
						            [
							            GetSelectedOptionToggle(SELECTOPTIONS::FORMAT_FBX,LOCTEXT("fbx", "fbx"))
						            ]
									+ SHorizontalBox::Slot()
									.Padding(2, 0, 60, 0)
									[
										GetSelectedOptionToggle(SELECTOPTIONS::FORMAT_OBJ,LOCTEXT("obj", "obj"))
									]
					                
							 ]
						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 //.Position(FVector2D(130.0f, 150.0f))
						 .Position(FVector2D(480.0f, 120.0f))
						 [
							 SNew(SBox)
							 [
								 SNew(SHorizontalBox)
								 + SHorizontalBox::Slot()
						           .AutoWidth()
						           .Padding(0, 0, 0, 0)
						           [
							           SNew(STextBlock)
							           .Text(FText::FromString("Texture Level"))
						               .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						               .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
						           ]

					             + SHorizontalBox::Slot()
						          .Padding(30, 0, 20, 0)
						          [
							          GetSelectedOptionToggle(SELECTOPTIONS::TWO_K, LOCTEXT("2K", "2K"))
						          ]
					              + SHorizontalBox::Slot()
						          .Padding(30, 0, 20, 0)
						          [
							          GetSelectedOptionToggle(SELECTOPTIONS::FOUR_K, LOCTEXT("4K", "4K"))
						          ]
					             + SHorizontalBox::Slot()
						         .Padding(30, 0, 20, 0)
						          [
							          //SNew(STextBlock)
							          GetSelectedOptionToggle(SELECTOPTIONS::EIGHT_K, LOCTEXT("8K", "8K"))
						          ]

					        ]
				         ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(130.0f, 160.0f))
						 [
							 SNew(SBox)
							 [
								 SNew(SHorizontalBox)
								 + SHorizontalBox::Slot()
						          .AutoWidth()
						          .Padding(2, 0, 0, 0)
						          [
							          SNew(STextBlock)
							          .Text(FText::FromString("Texture Type"))
						              .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						              .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
						          ]

					             + SHorizontalBox::Slot()
						           .Padding(85, 0, 0, 0)
						           [
							           GetSelectedOptionToggle(SELECTOPTIONS::DIFFUSE,LOCTEXT("Diffuse", "Diffuse"))
						           ]
					             + SHorizontalBox::Slot()
						           .Padding(85, 0, 0, 0)
						           [
							           GetSelectedOptionToggle(SELECTOPTIONS::NORMAL, LOCTEXT("Normal", "Normal"))
						           ]
							 ]
						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(480.0f, 160.0f))
						 [
							 SNew(SBox)
							 [
								 SNew(SHorizontalBox)
								 + SHorizontalBox::Slot()
						           .AutoWidth()
						           .Padding(2, 0, 0, 0)
						        [
							        SNew(STextBlock)
							        .Text(FText::FromString("Mesh Level"))
						            .ColorAndOpacity(FLinearColor(0.4352F, 0.3647F, 0.8862F))
						            .TextStyle(&FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
						        ]

					            + SHorizontalBox::Slot()
						         .Padding(38, 0, 20, 0)
						         [
							         GetSelectedOptionToggle(SELECTOPTIONS::HIGH, LOCTEXT("High", "High"))
						         ]
					            + SHorizontalBox::Slot()
						          .Padding(45, 0, 20, 0)
						         [
							         GetSelectedOptionToggle(SELECTOPTIONS::LOW,LOCTEXT("Low", "Low"))
						         ]
							 ]
						 ]



					 /////////////////
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(130.0f, 90.0f))
						 .Position(FVector2D(130.0f, 200.0f))
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
						 .Position(FVector2D(130.0f, 240.0f))
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
						 .Position(FVector2D(300.0f, 240.0f))
						 [
							 SAssignNew(pAssetPath,SEditableTextBox)
							 .Text(FText::FromString("Select the Path to Asset Folder"))

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(20.0f, 20.0f))
						 .Position(FVector2D(590.0f, 240.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						     .VAlign(VAlign_Center)
						     .OnClicked(this, &FRealBandUIManagerImpl::LaunchOpenFileDialog)

						 ]

					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(330.0f, 320.0f))
						 .Position(FVector2D(130.0f, 270.0f))
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
						 .Position(FVector2D(300.0f, 270.0f))
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
							
											 {
												 SelectedProject = Value;
							
												 ProjectsCombo->SetSelectedItem(Value);
												
							
												 ProjectsCombo->RefreshOptions();
												 pTextBlock->SetText(FText::FromString(*SelectedProject.Get()));
											 }
				
									 })

		
						       
									
						         .OnGenerateWidget_Lambda([this](TSharedPtr< FString > Value)
							     {

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
						 .Position(FVector2D(690.0f, 400.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						 .VAlign(VAlign_Center)
						 .Text(FText::FromString("Apply"))
						 .OnClicked(this, &FRealBandUIManagerImpl::ApplySettings)

						 ]
					 + SCanvas::Slot()
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Fill)
						 .Size(FVector2D(100.0f, 50.0f))
						 .Position(FVector2D(490.0f, 400.0f))
						 [
							 SNew(SButton)
							 .HAlign(HAlign_Center)
						 .VAlign(VAlign_Center)
						 .Text(FText::FromString("Reset"))
						 .OnClicked(this, &FRealBandUIManagerImpl::ResetSettings)

						 ]

					 //]// SBorder
					 
			      ]
				  ]// Settings Window
				//]
			    
			]


			]

			]

			];


		FSlateApplication::Get().AddWindow(pDialogMainWindow.ToSharedRef());

		pSettingsWindow->SetVisibility(EVisibility::Hidden);
		pSettingsBox->SetVisibility(EVisibility::Hidden);
		pFRealBandAssetLoader->SetVisibility(EVisibility::Collapsed);
		//
	//	const FSlateBrush* WarningIcon = FEditorStyle::GetBrush("C:\\Assets\\RealBand_bunddle_Asset\\RealBand_bunddle_Asset\\00001_oldCamera2\\00001_render_png_perspective_oldCamera2.png");
	//	

	//	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("TestStyle"));
	//	Style->SetContentRoot(IPluginManager::Get().FindPlugin("RealBandImporter")->GetBaseDir() / TEXT("Resources"));
	//	const FVector2D Icon20(20.0f, 20.0f);
	//	Style->Set("RealBandImporter.PluginAction", new IMAGE_BRUSH(TEXT("Asset"), Icon20));

	//	//FString IconDir("C:\\Assets\\Asset1.png");

	//	//FString DefaultImage = TEXT("C:\\Assets\\RealBand_bunddle_Asset\\RealBand_bunddle_Asset\\00001_oldCamera2\\00001_render_png_perspective_oldCamera2.png");
	//	FString DefaultImage = TEXT("C:\\Assets\\testAsset.png");
	//	UObject* ImageResource = FindObject<UObject>(ANY_PACKAGE, *DefaultImage);

	//	const FVector2D Icon40x40(40.0f, 40.0f);
	//	FSlateImageBrush* pBrush = new FSlateImageBrush(RootToContentDir(TEXT("Asset.png")), Icon40x40);
	////	pBrush->SetResourceObject(ImageResource);
 //       
	//	pGridPanel->AddSlot(1, 0)
	//		//	.VAlign(VAlign_Center)
	//		[
	//			SNew(SImage)
	//			//.Image(FEditorStyle::GetBrush(pGridPanel->GetIconStyleName(EPlatformIconSize::Normal)))
	//		.Image(pBrush)
	//		
	//		.DesiredSizeOverride(Icon40x40)
	//		
	//		]
	//	.Padding(20,0,40,0)
	//	    
	//	;
	//	
	//	//.Padding(0, 0, 10, 0)

	//	pGridPanel->AddSlot(2, 0)
	//		//	.VAlign(VAlign_Center)
	//		[
	//			SNew(SImage)
	//			//.Image(FEditorStyle::GetBrush(pGridPanel->GetIconStyleName(EPlatformIconSize::Normal)))
	//		.Image(WarningIcon)
	//		.DesiredSizeOverride(Icon40x40)
	//		];

		

		
		//

	}
	else
	{
	    bIsVisible = pDialogMainWindow->IsVisible();
		
    }

	pDialogMainWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
		{
			pDialogMainWindow.Reset();
		}));

	pSettingsWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
		{

			pSettingsWindow->SetVisibility(EVisibility::Collapsed);
			pSettingsBox->SetVisibility(EVisibility::Collapsed);
		}));

	


}


TSharedRef<SHorizontalBox> FRealBandUIManagerImpl::GetSelectedOptionToggle(SELECTOPTIONS type , const FText& Text)
{
	
	SAssignNew(pHBox, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()

		[
			SAssignNew(pChkBox, SCheckBox)
			//SNew(SCheckBox)
			.IsChecked(ECheckBoxState::Checked)
		    .BorderBackgroundColor(FSlateColor(FLinearColor::Gray))
		    //.BackgroundImage()

		.OnCheckStateChanged(this, &FRealBandUIManagerImpl::OnTypeChanged,type)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4.0f, 0.0f, 10.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(Text)

		];

	OptionsFormat.Add(pChkBox);
	return pHBox.ToSharedRef();
	/*return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		
		[
			SNew(SCheckBox)
			.IsChecked(ECheckBoxState::Checked)
		    
		.OnCheckStateChanged(this,&FRealBandUIManagerImpl::OnTypeChanged, type)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(4.0f, 0.0f, 10.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(Text)
		    
		];*/
}

void FRealBandUIManagerImpl::OnTypeChanged(ECheckBoxState CheckState, SELECTOPTIONS Type)
{
	const uint16 Mask = 1 << Type;
	//const uint16 TMask = 1 << TexType;
	switch (CheckState)
	{
	case ECheckBoxState::Checked:
		ObjUserPreference.ActiveTypeBitset |= Mask;
	//	AssetFormat = AssetFormat << Type;
		//ObjUserPreference.ActiveTypeBitset |= TMask;
		break;
	default:
		ObjUserPreference.ActiveTypeBitset &= ~Mask;
		//ObjUserPreference.ActiveTypeBitset &= ~TMask;
		break;
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
	pSettingsWindow->SetVisibility(EVisibility::Collapsed);
	pFRealBandAssetLoader->SetVisibility(EVisibility::Visible);
	pImport->SetVisibility(EVisibility::Visible);
	return FReply::Handled();
}

FReply FRealBandUIManagerImpl::LaunchSettings()
{
	pSettingsWindow->SetVisibility(EVisibility::Visible);
	pSettingsBox->SetVisibility(EVisibility::Visible);
	pImport->SetVisibility(EVisibility::Collapsed);
	pFRealBandAssetLoader->SetVisibility(EVisibility::Collapsed);
	pAssetFolderPath->SetText(FText::FromString("C:\\Assets\\RealBand_bunddle_Asset\\RealBand_bunddle_Asset"));
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

	
	pFRealBandAssetImporter->ImportSelectedAssets(AssetList, pAssetFolderPath->GetText(), ObjUserPreference);
	
	return FReply::Handled();
}


FReply FRealBandUIManagerImpl::ApplySettings()
{
	if(pFRealBandAssetLoader->GetVisibility() == EVisibility::Visible)
		return FReply::Handled();

	
	//bool isActive = false;
	
	//const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
	//isActive = ActiveTypeBitset & Mask;

	pSettingsWindow->SetVisibility(EVisibility::Collapsed);
	pSettingsBox->SetVisibility(EVisibility::Collapsed);
	pImport->SetVisibility(EVisibility::Visible);
	FText AssetFolderPath = pAssetFolderPath->GetText();
	FString fPath = AssetFolderPath.ToString();
	ObjUserPreference.FolderPath = std::string(TCHAR_TO_UTF8(*fPath));

	/*int AssetCount = 0;
	pFRealBandAssetImporter->GetAssetsCount(AssetCount);
	if(AssetCount <= 0 )*/
	pFRealBandAssetImporter->CreateTexturesFromAssets(AssetFolderPath);

	return FReply::Handled();
}

FReply FRealBandUIManagerImpl::ResetSettings()
{

	pAssetPath->ClearContent();
//	pAssetFolderPath->ClearContent();
	pAssetFolderPath->SetText(FText::FromString(""));
	
	for (TSharedPtr<SCheckBox> ptrChkBox : OptionsFormat)
	{
		ptrChkBox->SetIsChecked(ECheckBoxState::Unchecked);
	}
	
	ObjUserPreference.ActiveTypeBitset &= ~(0xFF);

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


void FRealBandUIManagerImpl::OnFolderPathChanged(const FText& ChangedText)
{
	pAssetFolderPath->SetText(ChangedText);
	//return FReply::Handled();
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