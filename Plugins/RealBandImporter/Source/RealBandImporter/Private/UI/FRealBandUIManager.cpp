// Copyright RealEye, Inc. All Rights Reserved.
#include "UI/FRealBandUIManager.h"

//Widgets

#include "Widgets/SWindow.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SDPIScaler.h"

TSharedPtr<FRealBandUIManagerImpl> FRealBandUIManager::Instance;
const FName RealBandTabName = "RealBandTab";

void FRealBandUIManagerImpl::Initialize()
{
	//Create the MainWindow 
	CreateWindow();

}

void FRealBandUIManager::Initialize()
{
	
	if (!Instance.IsValid())
	{
	//	//Instance = MakeUnique<FBridgeUIManagerImpl>();
		Instance = MakeShareable(new FRealBandUIManagerImpl);
		Instance->Initialize();
	}
}

void FRealBandUIManagerImpl::CreateWindow()
{
	TSharedPtr<SWindow> DialogMainWindow = SNew(SWindow)
		.Title(FText::FromString("RealBand"))
		.ClientSize(FVector2D(1000, 650))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		
		[
			SNew(SOverlay)// Stage selection

			+ SOverlay::Slot()
		      .VAlign(VAlign_Fill)
		      .HAlign(HAlign_Fill)
		      [
			      SNew(SBox)
			     .HAlign(HAlign_Fill)
		         .VAlign(VAlign_Fill)
		         [
			         SNew(SCanvas)
			        + SCanvas::Slot()
		             .HAlign(HAlign_Fill)
		             .VAlign(VAlign_Fill)
		             .Size(FVector2D(100.0f, 50.0f))
		             .Position(FVector2D(30.0f, 80.0f))
		             [
			             SNew(SButton)
			             .HAlign(HAlign_Center)
		                 .VAlign(VAlign_Center)
		                 .Text(FText::FromString("Settings"))
		 
		              ]
	                  + SCanvas::Slot()
		               .HAlign(HAlign_Fill)
		               .VAlign(VAlign_Fill)
		               .Size(FVector2D(100.0f, 50.0f))
		               .Position(FVector2D(30.0f, 180.0f))
		               [
			               SNew(SButton)
			              .HAlign(HAlign_Center)
		                  .VAlign(VAlign_Center)
		                  .Text(FText::FromString("General"))
					   ]
					  + SCanvas::Slot()
						  .HAlign(HAlign_Fill)
						  .VAlign(VAlign_Fill)
						  .Size(FVector2D(100.0f, 50.0f))
						  .Position(FVector2D(750.0f, 5.0f))
						  [
							  SNew(SButton)
							  .HAlign(HAlign_Center)
						  .VAlign(VAlign_Center)
						  .Text(FText::FromString("Import"))
						  ]
		         ]
	                
			  ]
			   
		];


	            


	            
			
				//SNew(SVerticalBox)
				//+ SVerticalBox::Slot()
		  //       .AutoHeight()
		  //       .Padding(20.0f, 40.0f, 20.0f, 10.0f)
		  //       [
			 //        SNew(SButton)
		  //          .Text(FText::FromString("Settings"))
		  //       ]

	   //        + SVerticalBox::Slot()
		  //      .AutoHeight()
		  //      .Padding(20.0f, 40.0f, 20.0f, 10.0f)
		  //      [
			 //       SNew(SButton)
			 //      .Text(FText::FromString("Local"))
		  //      ]

			   /*[
				   SNew(SScrollBox)
				   
				   + SScrollBox::Slot().HAlign(HAlign_Center)
				
			   ]*/


	

	//	[
	//		SNew(SButton)
	//		.ContentPadding(30)
	//	.Text(FText::FromString(TEXT("Settings")))
	//	.DesiredSizeScale(FVector2D(2.5, 2.5))

	//	]

		
		


	/*TSharedPtr<SWindow> DialogMainWindow = SNew(SWindow)
		.Title(FText::FromString(""))
		.ClientSize(FVector2D(1000, 450))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SCanvas)
			+ SCanvas::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Size(FVector2D(1000, 650))
		.Position(FVector2D(100, 100))
		[
			SNew(SBox)
			.WidthOverride(256)
		    .HeightOverride(100)
		    [
			    SNew(SButton)
			   .Text(FText::FromString(TEXT("Settings")))
		       .HAlign(HAlign_Center)
			]
		    
		 ]
		];*/



	FSlateApplication::Get().AddWindow(DialogMainWindow.ToSharedRef());

//	SAssignNew(Canvas, SCanvas).FArguments(Slot().SetSize(FVector2D(700, 700)));
	//if (Windows.Num() > 0)
	{
//		FVector2D MainWindowSize = Windows[0]->GetSizeInScreen();
		float DesiredWidth = 1650;
		float DesiredHeight = 900;

	//	SAssignNew(Canvas, SCanvas);
		
		
		
		//if (DesiredWidth < MainWindowSize.X && DesiredHeight < MainWindowSize.Y && Canvas->GetParentWindow().IsValid())
		{
			// If Bridge is docked as a tab, the parent window will be the main window
		
			//Canvas->Resize(FVector2D(DesiredWidth, DesiredHeight));
			//LocalBrowserDock->GetParentWindow()->MoveWindowTo(FVector2D((MainWindowSize.X - DesiredWidth) - 17, MainWindowSize.Y - DesiredHeight) / 2);
		}
	}

}