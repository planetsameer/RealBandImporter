#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockTab.h"

#include "Framework/Docking/TabManager.h"
#include "WebBrowser/Public/IWebBrowserWindow.h"
#include "ContentBrowserDelegates.h"

#include "C:\Program Files/Epic Games/UE_5.0/Engine/Plugins/Enterprise/DataSmithGLTFImporter/Source/DataSmithGLTFTranslator/Private/DatasmithGLTFImportOptions.h"
#include "C:\Program Files\Epic Games\UE_5.0\Engine\Plugins\Enterprise\DatasmithGLTFImporter\Source\DatasmithGLTFTranslator\Private\DatasmithGLTFImporter.h"
//#include "SWebBrowser.h"
//#include "UI/BrowserBinding.h"
//#include "Framework/MultiBox/MultiBoxBuilder.h"
//#include "Framework/Docking/TabManager.h"


class FRealBandUIManagerImpl : public TSharedFromThis<FRealBandUIManagerImpl>
{

private:
	TSharedPtr<SCanvas> pCanvas;
	TSharedPtr<SOverlay> pOverlay;
	TSharedPtr<SButton> pSettings;
	TSharedPtr<SBox> pSettingsBox;
	TSharedPtr<SButton> pLocal;
	TSharedPtr<SButton> pImport;
	TSharedPtr<SWindow> pDialogMainWindow;
	TSharedPtr<SWindow> pSettingsWindow;
	TSharedPtr<SBox> pBox;
	TSharedPtr<SBorder> pBorder;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ProjectsCombo;
	TSharedPtr<STextBlock> pTextBlock;
	TSharedPtr<SWebBrowser> WebBrowserWidget;
	TSharedPtr<SEditableTextBox> pAssetPath;
	FReply LaunchSettings();
	FReply ApplySettings();
	FReply LaunchOpenFileDialog();
	void HandleSourceComboChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	TArray<TSharedPtr <FString>> Array_Resolutions; 
	TSharedPtr<FString> SelectedProject;
	TSharedRef<SDockTab> CreatRealBandTab(const FSpawnTabArgs& Args);
	TSharedPtr<FString> FolderSelected;
	void HandlePathSelected(const FString&);
	FSetPathPickerPathsDelegate SetPathsDelegate;

	TStrongObjectPtr<UDatasmithGLTFImportOptions> ImportOptions;
	TStrongObjectPtr<UDatasmithGLTFImportOptions>& GetOrCreateGLTFImportOptions();
	TSharedPtr<class FDatasmithGLTFImporter> Importer;
public:
	void Initialize();
	void CreateWindow();
	
	TSharedPtr<SDockTab> LocalBrowserDock;
	TSharedPtr<SWindow> DragDropWindow;
	TSharedPtr<IWebBrowserWindow> Browser;
	FCreateBrowserWindowSettings WindowSettings;
};


class FRealBandUIManager
{
public:
	static void Initialize();
	static TSharedPtr<FRealBandUIManagerImpl> Instance;
};
