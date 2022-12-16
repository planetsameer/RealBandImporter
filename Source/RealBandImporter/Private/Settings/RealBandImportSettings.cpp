// Fill out your copyright notice in the Description page of Project Settings.


#include "RealBandImportSettings.h"
#include "CoreGlobals.h"
#include "CoreMinimal.h"

//Widgets
#include "Widgets/SWindow.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "UObject/ObjectSaveContext.h"


//Editor
#include "LevelEditor.h"
#include "PropertyEditor/Private/SDetailCategoryTableRow.h"
#if WITH_EDITOR
#include "ObjectEditorUtils.h"
#endif // WITH_EDITOR

// For the delegate AssetFolder update
#include "../Importer/FRealBandAssetImporter.h"

//Directory watch settings
#if WITH_REALBAND
#include "DerivedDataCacheInterface.h"
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "Interfaces/ITargetPlatform.h"
#endif //WITH_EDITOR

#include "Framework/Notifications/NotificationManager.h"

#include <string>
using namespace std;
#include <csignal>

#define LOCTEXT_NAMESPACE "RealBandImportSettingsWidget"


URealBandImportSettings::URealBandImportSettings()
{

}

URealBandImportSettings::~URealBandImportSettings()
{

}



void RealBandImportSettingsUI::OnPropertyChanged(UObject* ObjectBeingModified, FPropertyChangedEvent& PropertyChangedEvent)
{
	
	const FName propName = PropertyChangedEvent.GetPropertyName();
	const FName categoryName = FObjectEditorUtils::GetCategoryFName(PropertyChangedEvent.Property);
	if (SettingsDetailsView && pRealBandSettings && categoryName.GetStringLength() > 0 && propName != "None")
	{
		if (propName == "bImportFBX")
		{
			if (pRealBandSettings->bImportFBX)
			{
				SetOptionBit(ECheckBoxState::Checked, SELECTOPTIONS::FORMAT_FBX);
			}
			else
			{
				SetOptionBit(ECheckBoxState::Unchecked, SELECTOPTIONS::FORMAT_FBX);
			}
		}
		if (propName == "bImportGLM")
		{
			if (pRealBandSettings->bImportGLM)
			{
				SetOptionBit(ECheckBoxState::Checked, SELECTOPTIONS::FORMAT_GLM);
			}
			else
			{
				SetOptionBit(ECheckBoxState::Unchecked, SELECTOPTIONS::FORMAT_GLM);
			}
		}
		if (propName == "bImportOBJ")
		{
			if (pRealBandSettings->bImportOBJ)
			{
				SetOptionBit(ECheckBoxState::Checked, SELECTOPTIONS::FORMAT_OBJ);
			}
			else
			{
				SetOptionBit(ECheckBoxState::Unchecked, SELECTOPTIONS::FORMAT_OBJ);
			}
		}
		if (propName == "bHigh")
		{
			if (pRealBandSettings->bHigh)
			{
				SetOptionBit(ECheckBoxState::Checked, SELECTOPTIONS::HIGH);
			}
			else
			{
				SetOptionBit(ECheckBoxState::Unchecked, SELECTOPTIONS::HIGH);

			}
		}

		if (propName == "bLow")
		{
			if (pRealBandSettings->bLow)
			{
				SetOptionBit(ECheckBoxState::Checked, SELECTOPTIONS::LOW);

			}
			else
			{
				SetOptionBit(ECheckBoxState::Unchecked, SELECTOPTIONS::LOW);
			}
		}

		if (propName == "Path")
		{
			//if (ConvertedFString.GetDisplayNameEntry()->IsWide())
			if (FName(*(pRealBandSettings->AssetFolder.Path)).GetDisplayNameEntry()->IsWide())
			{
				WIDECHAR NewTextureNameString[NAME_SIZE];
				FName ConvertedFString = FName(*(pRealBandSettings->AssetFolder.Path));
				ConvertedFString.GetPlainWIDEString(NewTextureNameString);
				objUsrPreference->FolderPath = StringCast<WIDECHAR>(NewTextureNameString).Get();
			}
			else
			{
				std::string sFolderPath = std::string(TCHAR_TO_UTF8(*(pRealBandSettings->AssetFolder.Path)));
				std::wstring wFolderPath(sFolderPath.begin(), sFolderPath.end());
				objUsrPreference->FolderPath = wFolderPath;
			}

			
			//std::string sFolderPath = std::string(TCHAR_TO_UTF8(*(pRealBandSettings->AssetFolder.Path)));
			//std::wstring wFolderPath(sFolderPath.begin(), sFolderPath.end());
			//objUsrPreference->FolderPath = StringCast<WIDECHAR>(NewTextureNameString).Get();
		}

		if (propName == "b2K")
		{
			if (pRealBandSettings->b2K)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::TWOK);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::TWOK);
			}
		}

		if (propName == "b4K")
		{
			if (pRealBandSettings->b4K)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::FOURK);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::FOURK);
			}
		}

		if (propName == "b8K")
		{
			if (pRealBandSettings->b8K)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::EIGHTK);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::EIGHTK);
			}
		}

		if (propName == "bDiffuse")
		{
			if (pRealBandSettings->bDiffuse)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::TDIFFUSE);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::TDIFFUSE);
			}
		}

		if (propName == "bNormal")
		{
			if (pRealBandSettings->bNormal)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::TNORMAL);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, ::TEXTUREOPTIONS::TNORMAL);
			}
		}

		if (propName == "bTexHigh")
		{
			if (pRealBandSettings->bTexHigh)
			{
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::TEXHIGH);
			}
			else
			{
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::TEXHIGH);
			}
		}

		if (propName == "bTexLow")
		{
			if (pRealBandSettings->bTexLow)
			{
				//objUsrPreference->Format |= 1 << FORMATOPTIONS::TEXTURES;
				SetOptionTextureBit(ECheckBoxState::Checked, TEXTUREOPTIONS::TEXLOW);
			}
			else
			{
				//objUsrPreference->Format &= 1 << FORMATOPTIONS::TEXTURES;;
				SetOptionTextureBit(ECheckBoxState::Unchecked, TEXTUREOPTIONS::TEXLOW);
			}
		}
		
		pRealBandSettings->gUserPreference.ActiveTypeBitset = objUsrPreference->ActiveTypeBitset;
		pRealBandSettings->gUserPreference.ActiveTextypeBitset = objUsrPreference->ActiveTextypeBitset;
		pRealBandSettings->SaveConfig(CPF_Config, nullptr, GConfig, false);;
		
		
	}

	if (SettingsDetailsView && pRealBandSettings)
	{
		pRealBandSettings->StartDirectoryWatch(objUsrPreference->FolderPath.c_str());
	}
}

void RealBandImportSettingsUI::SetOptionBit(ECheckBoxState CheckState, SELECTOPTIONS Type)
{
	const uint16 Mask = 1 << Type;
	//const uint16 TMask = 1 << TexType;
	switch (CheckState)
	{
	case ECheckBoxState::Checked:
		objUsrPreference->ActiveTypeBitset |= Mask;
		break;
	case ECheckBoxState::Unchecked:
		objUsrPreference->ActiveTypeBitset &= ~Mask;
		break;
	default:
		objUsrPreference->ActiveTypeBitset &= ~Mask;
		break;
	}
}


void RealBandImportSettingsUI::SetOptionTextureBit(ECheckBoxState CheckState, TEXTUREOPTIONS Type)
{
	const uint16 Mask = 1 << Type;
	//const uint16 TMask = 1 << TexType;
	switch (CheckState)
	{
	case ECheckBoxState::Checked:
		objUsrPreference->ActiveTextypeBitset |= Mask;
		break;
	case ECheckBoxState::Unchecked:
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
		break;
	default:
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
		break;
	}
}

RealBandImportSettingsUI::~RealBandImportSettingsUI()
{
	pRealBandSettings = nullptr;
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(OnPropertyChangedHandle);
}

void RealBandImportSettingsUI::Construct(const FArguments& InArgs)
{
	objUsrPreference = InArgs._ObjPreference;
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	pRealBandSettings = GetMutableDefault<URealBandImportSettings>();
		
	TSharedRef<FRealBandAssetImporter> pRealBandAssetImporter(new FRealBandAssetImporter());
	pRealBandSettings->UpdateAssetFolderDelegate.BindSP(pRealBandAssetImporter, &FRealBandAssetImporter::CreateTexturesFromAssets);
	
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = true;
	//DetailsViewArgs.NotifyHook = this;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bShowPropertyMatrixButton = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.ViewIdentifier = NAME_None;
	SettingsDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	//pRealBandSettings = GetMutableDefault<URealBandImportSettings>();
	SettingsDetailsView->SetObject(pRealBandSettings);
	SettingsDetailsView->OnFinishedChangingProperties().AddSP(this, &RealBandImportSettingsUI::OnFinishedChangingProperties);
	//SettingsDetailsView->SetOnObjectArrayChanged(FOnObjectArrayChanged::CreateSP(this, &RealBandImportSettingsUI::OnPropertyChanged));
	//pRealBandSettings->NotifyPostChange(EPropertyChangeType::ValueSet);

	FCoreUObjectDelegates::FOnObjectPropertyChanged::FDelegate OnPropertyChangedDelegate = FCoreUObjectDelegates::FOnObjectPropertyChanged::FDelegate::CreateRaw(this, &RealBandImportSettingsUI::OnPropertyChanged);
	OnPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.Add(OnPropertyChangedDelegate);

	

	ChildSlot
		[

			SAssignNew(pSettingsWindow, SWindow)
			.Title(FText::FromString("Settings"))
		.Style(&FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window"))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(900, 600))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
				 SAssignNew(pSettingsCanvasBox, SCanvas)
					 + SCanvas::Slot()
				      .HAlign(HAlign_Fill)
				      .VAlign(VAlign_Fill)
	                  .Size(FVector2D(750.0f, 600.0f))
				      .Position(FVector2D(100.0f, 15.0f))
				      [
						  SNew(SVerticalBox)
						  + SVerticalBox::Slot()
				        [
						  SNew(SSplitter)
						  .Orientation(EOrientation::Orient_Vertical)
				           + SSplitter::Slot()
				            .Value(0.8f)
				            [
					            SNew(SSplitter)
					            .Orientation(EOrientation::Orient_Horizontal)
				                + SSplitter::Slot()
				                .Value(0.5f)
				                [
					                SNew(SWidgetSwitcher)
					                + SWidgetSwitcher::Slot()
				                      
				                      .HAlign(HAlign_Fill)
				                      .VAlign(VAlign_Fill)
				                    [
					                   SettingsDetailsView.ToSharedRef()
				                    ]
				                 ]
				             ]
						]
						 
					   ]
						 	 + SCanvas::Slot()
				 		 .HAlign(HAlign_Fill)
				 		 .VAlign(VAlign_Fill)
				 		 .Size(FVector2D(100.0f, 50.0f))
				 		 .Position(FVector2D(490.0f, 600.0f))
				 		 [
				 			 SNew(SButton)
				 			 .HAlign(HAlign_Center)
				 		 .VAlign(VAlign_Center)
				 		 .Text(FText::FromString("Reset"))
				 		 .OnClicked(this, &RealBandImportSettingsUI::ResetSettings)
				 		 .ToolTipText(LOCTEXT("ResetButtonTooltip", "Clear all settings"))
				 		 ]
		]
	

		];

//	FCoreUObjectDelegates::FOnObjectPropertyChanged::FDelegate OnPropertyChangedDelegate = FCoreUObjectDelegates::FOnObjectPropertyChanged::FDelegate::CreateRaw(this, &RealBandImportSettingsUI::OnPropertyChanged);
//	OnPropertyChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.Add(OnPropertyChangedDelegate);

	// Inital turn all the options ON
	pRealBandSettings->bImportFBX = true;
	pRealBandSettings->bImportGLM = true;
	pRealBandSettings->bImportOBJ = true;
	pRealBandSettings->bHigh = true;
	pRealBandSettings->bLow = true;
	pRealBandSettings->bDiffuse = true;
	pRealBandSettings->bNormal = true;
	pRealBandSettings->b2K = true;
	pRealBandSettings->b4K = true;
	pRealBandSettings->b8K = true;
	pRealBandSettings->bTexHigh = true;
	pRealBandSettings->bTexLow = true;


	
	pRealBandSettings->LoadConfig();
    // Initialise the Properties from the saved config
	if (FName(*(pRealBandSettings->AssetFolder.Path)).GetDisplayNameEntry()->IsWide())
	{
		WIDECHAR NewTextureNameString[NAME_SIZE];
		FName ConvertedFString = FName(*(pRealBandSettings->AssetFolder.Path));
		ConvertedFString.GetPlainWIDEString(NewTextureNameString);
		objUsrPreference->FolderPath = StringCast<WIDECHAR>(NewTextureNameString).Get();
	}
	else
	{
		std::string sFolderPath = std::string(TCHAR_TO_UTF8(*(pRealBandSettings->AssetFolder.Path)));
		std::wstring wFolderPath(sFolderPath.begin(), sFolderPath.end());
		objUsrPreference->FolderPath = wFolderPath;
	}
	
	UpdateUserPreferences(objUsrPreference);
//	objUsrPreference->ActiveTypeBitset = pRealBandSettings->gUserPreference.ActiveTypeBitset;
//	objUsrPreference->ActiveTextypeBitset = pRealBandSettings->gUserPreference.ActiveTextypeBitset;
	//pSettingsWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
	//	{
	//		pSettingsWindow.Reset();
	//	}));
}


void RealBandImportSettingsUI::UpdateUserPreferences(USRPREFERENCE* opUsrPreferences)
{
	if (pRealBandSettings->bImportFBX)
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_FBX;
		opUsrPreferences->ActiveTypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_FBX;
		objUsrPreference->ActiveTypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bImportGLM)
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
		opUsrPreferences->ActiveTypeBitset |= Mask;

	}
	else
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_GLM;
		objUsrPreference->ActiveTypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bImportOBJ)
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_OBJ;
		opUsrPreferences->ActiveTypeBitset |= Mask;

	}
	else
	{
		const uint16 Mask = 1 << SELECTOPTIONS::FORMAT_OBJ;
		objUsrPreference->ActiveTypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bHigh)
	{
		const uint16 Mask = 1 << SELECTOPTIONS::HIGH;
		opUsrPreferences->ActiveTypeBitset |= Mask;

	}
	else
	{
		const uint16 Mask = 1 << SELECTOPTIONS::HIGH;
		objUsrPreference->ActiveTypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bLow)
	{
		const uint16 Mask = 1 << SELECTOPTIONS::LOW;
		opUsrPreferences->ActiveTypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << SELECTOPTIONS::LOW;
		objUsrPreference->ActiveTypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bDiffuse)
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TDIFFUSE;
		opUsrPreferences->ActiveTextypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TDIFFUSE;
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
	}

	if (pRealBandSettings->bNormal)
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TNORMAL;
		opUsrPreferences->ActiveTextypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TNORMAL;
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
	}

	if (pRealBandSettings->b2K)
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TWOK;
		opUsrPreferences->ActiveTextypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::TWOK;
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
	}

	if (pRealBandSettings->b4K)
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::FOURK;
		opUsrPreferences->ActiveTextypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::FOURK;
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
	}

	if (pRealBandSettings->b8K)
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::EIGHTK;
		opUsrPreferences->ActiveTextypeBitset |= Mask;
	}
	else
	{
		const uint16 Mask = 1 << TEXTUREOPTIONS::EIGHTK;
		objUsrPreference->ActiveTextypeBitset &= ~Mask;
	}
	
}

void RealBandImportSettingsUI::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		/*FGraphPanelSelectionSet CurrentSelection = GetSelectedNodes();
		if (CurrentSelection.Num() == 1)
		{
			for (FGraphPanelSelectionSet::TConstIterator It(CurrentSelection); It; ++It)
			{
				UEnvironmentQueryGraphNode_Test* TestNode = Cast<UEnvironmentQueryGraphNode_Test>(*It);
				UEnvironmentQueryGraphNode_Option* ParentNode = TestNode ? Cast<UEnvironmentQueryGraphNode_Option>(TestNode->ParentNode) : nullptr;

				if (ParentNode)
				{
					ParentNode->CalculateWeights();
					break;
				}
			}
		}*/
	}
}

FReply RealBandImportSettingsUI::SaveSettings()
{
	//pRealBandSettings->SaveConfig();
	pRealBandSettings->SaveConfig(CPF_Config, nullptr, GConfig, false);
	SettingsDetailsView.Reset();
	pSettingsWindow.Reset();
	
	return FReply::Handled();
}

FReply RealBandImportSettingsUI::SaveConfig(USRPREFERENCE & iUserPreference)
{
	iUserPreference.ActiveTextypeBitset = objUsrPreference->ActiveTextypeBitset;
	iUserPreference.ActiveTypeBitset = objUsrPreference->ActiveTypeBitset;
	pRealBandSettings->SaveConfig(CPF_Config, nullptr, GConfig, false);
	return FReply::Handled();
}

FReply RealBandImportSettingsUI::ResetSettings()
{

	pRealBandSettings->bImportFBX = false;
	pRealBandSettings->bImportGLM = false;
	pRealBandSettings->bImportOBJ = false;
	pRealBandSettings->bHigh = false;
	pRealBandSettings->bLow = false;
	pRealBandSettings->bDiffuse = false;
	pRealBandSettings->bNormal = false;
	pRealBandSettings->b2K = false;
	pRealBandSettings->b4K = false;
	pRealBandSettings->b8K = false;
	pRealBandSettings->bTexHigh = false;
	pRealBandSettings->bTexLow = false;
	pRealBandSettings->AssetFolder.Path = TEXT("");
	pRealBandSettings->connectPath = FString(TEXT(""));
	objUsrPreference->ActiveTextypeBitset &= ~(0xFFFF);
	objUsrPreference->ActiveTypeBitset &= ~(0xFFFF);

	return FReply::Handled();
}

void URealBandImportSettings::StartDirectoryWatch(const FString& FilePath)
{
#if WITH_REALBAND
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(RealBandDirectoryWatcher::NAME_DirectoryWatcher);
	if (IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get())
	{
		FString FolderPath = FPaths::GetPath(FilePath);

		// Unregister watched folder since ocio config path has changed..
		StopDirectoryWatch();
		
		WatchedDirectoryInfo.FolderPath = MoveTemp(FolderPath);
		{
			DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
				WatchedDirectoryInfo.FolderPath, IDirectoryWatcher::FDirectoryChanged::CreateUObject(this, &URealBandImportSettings::AssetFolderPathChangedEvent, WatchedDirectoryInfo.FolderPath),
				WatchedDirectoryInfo.DirectoryWatcherHandle,
				/*Flags*/ 0
			);
		}
	}
#endif
}

void URealBandImportSettings::StopDirectoryWatch()
{
#if WITH_REALBAND
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(RealBandDirectoryWatcher::NAME_DirectoryWatcher);
	if (IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get())
	{
		if (WatchedDirectoryInfo.DirectoryWatcherHandle.IsValid())
		{
			DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(WatchedDirectoryInfo.FolderPath, WatchedDirectoryInfo.DirectoryWatcherHandle);
			WatchedDirectoryInfo.FolderPath.Empty();
		}
	}
#endif
}


void URealBandImportSettings::AssetFolderPathChangedEvent(const TArray<FFileChangeData>& InFileChanges, const FString InFileMountPath)
{
#if WITH_REALBAND
	// We want to stop reacting to these events while the message is still up.
	if (WatchedDirectoryInfo.RawConfigChangedToast.IsValid())
	{
		return;
	}
	for (const FFileChangeData& FileChangeData : InFileChanges)
	{
		const FString FileExtension = FPaths::GetExtension(FileChangeData.Filename);
		if (FileExtension.IsEmpty() || !RealBandDirectoryWatcher::TextureExtensions.Contains(FileExtension))
		{
			continue;
		}

		const FText DialogBody = FText::Format(LOCTEXT("RealBandAssetFolderChanged",
			"Files added or removed from the Asset Folder. \
			\nWould you like to reload '{0}' Local Tab?"),
			FText::FromString(GetName()));

		const FText ReloadRawConfigText = LOCTEXT("ReloadRawConfigConfirm", "Reload");
		const FText IgnoreReloadRawConfigText = LOCTEXT("IgnoreReloadRawConfig", "Ignore");


		FSimpleDelegate OnReloadRawConfig = FSimpleDelegate::CreateLambda([this]() { OnToastCallback(true); });
		FSimpleDelegate OnIgnoreReloadRawConfig = FSimpleDelegate::CreateLambda([this]() { OnToastCallback(false); });

		FNotificationInfo Info(DialogBody);
		Info.bFireAndForget = false;
		Info.bUseLargeFont = false;
		Info.bUseThrobber = false;
		Info.bUseSuccessFailIcons = false;
		Info.ButtonDetails.Add(FNotificationButtonInfo(ReloadRawConfigText, FText(), OnReloadRawConfig));
		Info.ButtonDetails.Add(FNotificationButtonInfo(IgnoreReloadRawConfigText, FText(), OnIgnoreReloadRawConfig));

		WatchedDirectoryInfo.RawConfigChangedToast = FSlateNotificationManager::Get().AddNotification(Info);

		if (WatchedDirectoryInfo.RawConfigChangedToast.IsValid())
		{
			WatchedDirectoryInfo.RawConfigChangedToast.Pin()->SetCompletionState(SNotificationItem::CS_Pending);
		}

		break;
	}

#endif

}

void URealBandImportSettings::OnToastCallback(bool bInReloadAssets)
{
	if (WatchedDirectoryInfo.RawConfigChangedToast.IsValid())
	{
		WatchedDirectoryInfo.RawConfigChangedToast.Pin()->SetCompletionState(SNotificationItem::CS_Success);
		WatchedDirectoryInfo.RawConfigChangedToast.Pin()->ExpireAndFadeout();
		WatchedDirectoryInfo.RawConfigChangedToast.Reset();
	}

	if (bInReloadAssets)
	{
		//std::wstring sFolderPath = std::wstring(TCHAR_TO_UTF8(*(AssetFolder.Path)));
		std::wstring sFolderPath = std::wstring((*(AssetFolder.Path)));
		UpdateAssetFolderDelegate.Execute(FText::FromString(sFolderPath.c_str()));
// 		UpdateAssetFolderDelegate.Execute(FText::FromString(gUserPreference.FolderPath.c_str()));
//		UpdateAssetFolderDelegate.Execute(FText::FromString(TEXT("Invalid Input Folder")));
	//	ReloadExistingColorspaces();
	}
}


#undef LOCTEXT_NAMESPACE