// Copyright RealEye, Inc. All Rights Reserved.
#include "FRealBandUIManager.h"

//Widgets

#include "Widgets/SWindow.h"
TSharedPtr<FRealBandUIManagerImpl> FRealBandUIManager::Instance;

void FBridgeUIManager::Initialize()
{
	
	if (!Instance.IsValid())
	{
	//	//Instance = MakeUnique<FBridgeUIManagerImpl>();
		Instance = MakeShareable(new FBridgeUIManagerImpl);
		Instance->Initialize();
	}
}

void FBridgeUIManagerImpl::Initialize()
{
	FBridgeStyle::Initialize();
	SetupMenuItem();
}