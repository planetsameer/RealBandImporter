// Fill out your copyright notice in the Description page of Project Settings.


#include "RealBandImportSettings.h"
//Widgets
#include "Widgets/SWindow.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

//Editor
#include "LevelEditor.h"
#include "PropertyEditor/Private/SDetailCategoryTableRow.h"
#if WITH_EDITOR
#include "ObjectEditorUtils.h"
#endif // WITH_EDITOR

#include <string>
using namespace std;

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
		
		pRealBandSettings->SaveConfig(CPF_Config, nullptr, GConfig, false);;
		pRealBandSettings->gUserPreference = *objUsrPreference;

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
}

void RealBandImportSettingsUI::Construct(const FArguments& InArgs)
{
	objUsrPreference = InArgs._ObjPreference;
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	pRealBandSettings = GetMutableDefault<URealBandImportSettings>();
	
	
	
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
		.SizingRule(ESizingRule::Autosized)
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
	
	objUsrPreference->ActiveTypeBitset = pRealBandSettings->gUserPreference.ActiveTypeBitset;
	objUsrPreference->ActiveTextypeBitset = pRealBandSettings->gUserPreference.ActiveTextypeBitset;
	//pSettingsWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window)
	//	{
	//		pSettingsWindow.Reset();
	//	}));
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
	SettingsDetailsView.Reset();
	pSettingsWindow.Reset();
	
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




#undef LOCTEXT_NAMESPACE