#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
//#include "SWebBrowser.h"
//#include "UI/BrowserBinding.h"
//#include "Framework/MultiBox/MultiBoxBuilder.h"
//#include "Framework/Docking/TabManager.h"

class FRealBandUIManagerImpl : public TSharedFromThis<FRealBandUIManagerImpl>
{
public:
	void Initialize();

};


class FRealBandUIManager
{
public:
	static void Initialize();
	void Test();
	static TSharedPtr<FRealBandUIManagerImpl> Instance;
};
