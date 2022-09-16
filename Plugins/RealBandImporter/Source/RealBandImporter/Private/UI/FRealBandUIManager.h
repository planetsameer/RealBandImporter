#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCanvas.h"
//#include "SWebBrowser.h"
//#include "UI/BrowserBinding.h"
//#include "Framework/MultiBox/MultiBoxBuilder.h"
//#include "Framework/Docking/TabManager.h"

class FRealBandUIManagerImpl : public TSharedFromThis<FRealBandUIManagerImpl>
{

private:
	TSharedPtr<SCanvas> Canvas;
public:
	void Initialize();
	void CreateWindow();
	TSharedPtr<SDockTab> LocalBrowserDock;
	TSharedPtr<SWindow> DragDropWindow;
};


class FRealBandUIManager
{
public:
	static void Initialize();
	static TSharedPtr<FRealBandUIManagerImpl> Instance;
};
