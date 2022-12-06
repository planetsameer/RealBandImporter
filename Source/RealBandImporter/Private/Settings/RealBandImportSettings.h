//Copyright(C) 2022 VAXPORT all rights reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RealBandCommon.h" 
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "RealBandCommon.h"

#include "RealBandImportSettings.generated.h"

class SWindow;
class SBox;
class SCanvas;
struct FPropertyChangedEvent;
/**
 * 
 */
UCLASS(Config = Editor)
class   URealBandImportSettings : public UObject, public FNotifyHook
{
	GENERATED_BODY()
public:

	URealBandImportSettings();
	virtual ~URealBandImportSettings();
	/** Import Formats */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (FFilePath))
		FDirectoryPath AssetFolder;

	UPROPERTY(Config, DisplayName = "Uri", EditAnywhere, Category = "Unreal Plugin Connect")
	FString  connectPath;

	UPROPERTY(Config, DisplayName = "FBX", EditAnywhere, Category = "Mesh Settings")
		bool bImportFBX;
		
	UPROPERTY(Config, DisplayName = "GLM", EditAnywhere, Category = "Mesh Settings")
		bool bImportGLM;

	UPROPERTY(Config, DisplayName = "OBJ", EditAnywhere, Category = "Mesh Settings")
		bool bImportOBJ;

	UPROPERTY(Config, DisplayName = "High", EditAnywhere, Category = "Mesh Settings")
		bool bHigh;

	UPROPERTY(Config, DisplayName = "Low", EditAnywhere, Category = "Mesh Settings")
		bool bLow;

	/** Import Textures */
	UPROPERTY(Config, DisplayName = "Diffuse", EditAnywhere, Category = "Texture Settings")
		bool bDiffuse;

	UPROPERTY(Config, DisplayName = "Normal", EditAnywhere, Category = "Texture Settings")
		bool bNormal;

	UPROPERTY(Config, DisplayName = "2K", EditAnywhere, Category = "Texture Settings")
		bool b2K;

	UPROPERTY(Config, DisplayName = "4K", EditAnywhere, Category = "Texture Settings")
		bool b4K;

	UPROPERTY(Config, DisplayName = "8k", EditAnywhere, Category = "Texture Settings")
		bool b8K;

	UPROPERTY(Config, DisplayName = "High", EditAnywhere, Category = "Texture Settings")
		bool bTexHigh;

	UPROPERTY(Config, DisplayName = "Low", EditAnywhere, Category = "Texture Settings")
		bool bTexLow;

	//UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (FFilePath))
	//FDirectoryPath AssetFolder;

	//UPROPERTY(Config, DisplayName = "Uri", EditAnywhere, Category = "Unreal Plugin Connect")
	//FString  connectPath;

//	UPROPERTY(Config, DisplayName = "Path", EditAnywhere, Category = "ContentDir")
//		FString FolderPath;;
//	TSharedPtr<SButton> UpdateRevisionButton;
	USRPREFERENCE gUserPreference;
	
	
private:
	
};


class RealBandImportSettingsUI : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(RealBandImportSettingsUI) {}

	/** bit base number for options selected */
	SLATE_ARGUMENT(USRPREFERENCE *, ObjPreference)
	//	SLATE_ARGUMENT(FPathPickerConfig, AssetPathConfig)

	SLATE_END_ARGS()
public:

		void Construct(const FArguments& InArgs);
		TSharedPtr<SWindow> pSettingsWindow;
				FReply SaveSettings();
		FReply ResetSettings();
		void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
		void OnPropertyViewObjectArrayChanged(const FString& InTitle, const TArray<UObject*>& InObjects);
		
	    
		USRPREFERENCE *objUsrPreference;
		virtual ~RealBandImportSettingsUI();
		void OnPropertyChanged(UObject* ObjectBeingModified, FPropertyChangedEvent& PropertyChangedEvent);
		FDelegateHandle OnPropertyChangedHandle;
private:
	void SetOptionBit(ECheckBoxState CheckState, SELECTOPTIONS Type);
	void SetOptionTextureBit(ECheckBoxState CheckState, TEXTUREOPTIONS Type);
	TSharedPtr<class IDetailsView> SettingsDetailsView;
	TSharedPtr<SCanvas> pSettingsCanvasBox;
	URealBandImportSettings* pRealBandSettings;

};
