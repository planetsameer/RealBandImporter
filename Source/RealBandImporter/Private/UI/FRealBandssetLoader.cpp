

#include "FRealBandAssetLoader.h"

//Slate
#include "Framework/Application/SlateApplication.h"
#include "SAssetView.h"
#include "SAssetSearchBox.h"
#include "Editor.h"
// Widgets
#include "Widgets/Input/SComboButton.h"

#include "FrontendFilters.h"

#define LOCTEXT_NAMESPACE "RealBandAssetPreviewWidget"


FRealBandAssetLoader::~FRealBandAssetLoader()
{

}


void FRealBandAssetLoader::SetAssetFolder(const FString & FolderPath)
{

}

void FRealBandAssetLoader::Construct(const FArguments& InArgs)
{
	OnAssetsActivated = InArgs._AssetPickerConfig.OnAssetsActivated;
	OnAssetSelected = InArgs._AssetPickerConfig.OnAssetSelected;
	OnAssetEnterPressed = InArgs._AssetPickerConfig.OnAssetEnterPressed;
	

	for (auto DelegateIt = InArgs._AssetPickerConfig.SyncToAssetsDelegates.CreateConstIterator(); DelegateIt; ++DelegateIt)
	{
		if ((*DelegateIt) != NULL)
		{
			(**DelegateIt) = FSyncToAssetsDelegate::CreateSP(this, &FRealBandAssetLoader::SyncToAssets);
		}
	}

	if (InArgs._AssetPickerConfig.bFocusSearchBoxWhenOpened)
	{
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &FRealBandAssetLoader::SetFocusPostConstruct));
	}

	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);
	ChildSlot
		[
			VerticalBox
		];

	FName AssetPath(InArgs._AssetPathConfig.DefaultPath);
	CurrentSourcesData = FSourcesData(InArgs._AssetPickerConfig.Filter.PackagePaths, InArgs._AssetPickerConfig.Collections);
	// Make RealBand-specific filter
	CurrentBackendFilter = InArgs._AssetPickerConfig.Filter;
	CurrentBackendFilter.PackagePaths.Reset();
	// Filters
		TArray<UClass*> FilterClassList;
	for (auto Iter = CurrentBackendFilter.ClassNames.CreateIterator(); Iter; ++Iter)
	{
		FName ClassName = (*Iter);
		UClass* FilterClass = FindObject<UClass>(ANY_PACKAGE, *ClassName.ToString());
		if (FilterClass)
		{
			FilterClassList.AddUnique(FilterClass);
		}
	}

	VerticalBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 2)
		[
			SNew(SHorizontalBox)
			// Search
		    + SHorizontalBox::Slot()
		      //.Padding(0, 1, 0, 0)
		      .FillWidth(1.0f)
		      [
				  SAssignNew(SearchBoxPtr, SAssetSearchBox)
			      //.HintText(NSLOCTEXT("ContentBrowser", "SearchBoxHint", "Search Assets"))
		          .OnTextCommitted(this, &FRealBandAssetLoader::OnSearchBoxCommitted)
				  .OnTextChanged(this, &FRealBandAssetLoader::OnSearchBoxChanged)
		          .DelayChangeNotificationsWhileTyping(true)
		          .OnKeyDownHandler(this, &FRealBandAssetLoader::HandleKeyDownFromSearchBox)
		      ]
	        + SHorizontalBox::Slot()
              .AutoWidth()
				[
					SAssignNew(FilterComboButtonPtr, SComboButton)
					.ComboButtonStyle(&FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>("SimpleComboButton"))
				    .ToolTipText(LOCTEXT("AddFilterToolTip", "Add an asset filter."))
				    .HasDownArrow(false)
				    .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ContentBrowserFiltersCombo")))
				    .ButtonContent()
				     [
					     SNew(SImage)
					    .ColorAndOpacity(FSlateColor::UseForeground())
				        .Image(FAppStyle::Get().GetBrush("Icons.Filter"))
				     ]
			    ]
		  ];
	
	FrontendFilters = MakeShareable(new FAssetFilterCollectionType());

	VerticalBox->AddSlot()
		.FillHeight(1.f)
		[
			SAssignNew(AssetViewPtr, SAssetView)
			.InitialViewType(EAssetViewType::Tile)
		    .InitialSourcesData(CurrentSourcesData)
		    .InitialThumbnailSize(EThumbnailSize::Small)
		    .OnItemsActivated(this, &FRealBandAssetLoader::HandleItemsActivated)
		    .OnSearchOptionsChanged(this, &FRealBandAssetLoader::HandleSearchSettingsChanged)
		       
		];

	if ( AssetViewPtr.IsValid() )
	{
		const FSourcesData SourcesData = AssetViewPtr->GetSourcesData();;
		TextFilter = MakeShareable(new FFrontendFilter_Text());
		bool bClassNamesProvided = (InArgs._AssetPickerConfig.Filter.ClassNames.Num() != 1);
		TextFilter->SetIncludeClassName(bClassNamesProvided || AssetViewPtr->IsIncludingClassNames());
		TextFilter->SetIncludeAssetPath(AssetViewPtr->IsIncludingAssetPaths());
		TextFilter->SetIncludeCollectionNames(AssetViewPtr->IsIncludingCollectionNames());
	
		AssetViewPtr->RequestSlowFullListRefresh();
		AssetViewPtr->SetSourcesData(CurrentSourcesData);
		//AssetViewPtr->SetUserSearching(true);
	}

	
}

EActiveTimerReturnType FRealBandAssetLoader::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime)
{
	if (SearchBoxPtr.IsValid())
	{
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchBoxPtr.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
		WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchBoxPtr);

		return EActiveTimerReturnType::Stop;
	}
	
	return EActiveTimerReturnType::Continue;
}


void FRealBandAssetLoader::HandleSearchSettingsChanged()
{
//	bool bClassNamesProvided = (FilterListPtr.IsValid() ? FilterListPtr->GetInitialClassFilters().Num() != 1 : false);
	bool bClassNamesProvided = true;
	TextFilter->SetIncludeClassName(bClassNamesProvided || AssetViewPtr->IsIncludingClassNames());
	TextFilter->SetIncludeAssetPath(AssetViewPtr->IsIncludingAssetPaths());
	TextFilter->SetIncludeCollectionNames(AssetViewPtr->IsIncludingCollectionNames());
}

void FRealBandAssetLoader::OnSearchBoxChanged(const FText& InSearchText)
{
	SetSearchBoxText(InSearchText);
}

void FRealBandAssetLoader::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	SetSearchBoxText(InSearchText);

	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FContentBrowserItem> SelectionSet = AssetViewPtr->GetSelectedFileItems();
		if (SelectionSet.Num() == 0)
		{
			AssetViewPtr->AdjustActiveSelection(1);
			SelectionSet = AssetViewPtr->GetSelectedFileItems();
		}
		HandleItemsActivated(SelectionSet, EAssetTypeActivationMethod::Opened);
	}
}

void FRealBandAssetLoader::SetSearchBoxText(const FText& InSearchText)
{
	// Has anything changed? (need to test case as the operators are case-sensitive)
	if (!InSearchText.ToString().Equals(TextFilter->GetRawFilterText().ToString(), ESearchCase::CaseSensitive))
	{
		TextFilter->SetRawFilterText(InSearchText);
		if (InSearchText.IsEmpty())
		{
			FrontendFilters->Remove(TextFilter);
			AssetViewPtr->SetUserSearching(false);
		}
		else
		{
			FrontendFilters->Add(TextFilter);
			AssetViewPtr->SetUserSearching(true);
		}
	}
}

void FRealBandAssetLoader::HandleItemsActivated(TArrayView<const FContentBrowserItem> ActivatedItems, EAssetTypeActivationMethod::Type ActivationMethod)
{
	FContentBrowserItem FirstActivatedFolder;

	TArray<FAssetData> ActivatedAssets;
	for (const FContentBrowserItem& ActivatedItem : ActivatedItems)
	{
		if (ActivatedItem.IsFile())
		{
			FAssetData ItemAssetData;
			if (ActivatedItem.Legacy_TryGetAssetData(ItemAssetData))
			{
				ActivatedAssets.Add(MoveTemp(ItemAssetData));
			}
		}

		if (ActivatedItem.IsFolder() && !FirstActivatedFolder.IsValid())
		{
			FirstActivatedFolder = ActivatedItem;
		}
	}

	if (ActivatedAssets.Num() == 0)
	{
		return;
	}

	if (ActivationMethod == EAssetTypeActivationMethod::Opened)
	{
		OnAssetEnterPressed.ExecuteIfBound(ActivatedAssets);
	}

	OnAssetsActivated.ExecuteIfBound(ActivatedAssets, ActivationMethod);
}

FReply FRealBandAssetLoader::HandleKeyDownFromSearchBox(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	// Hide the filter list
	if (FilterComboButtonPtr.IsValid())
	{
		FilterComboButtonPtr->SetIsOpen(false);
	}

	// Up and down move thru the filtered list
	int32 SelectionDelta = 0;

	if (InKeyEvent.GetKey() == EKeys::Up)
	{
		SelectionDelta = -1;
	}
	else if (InKeyEvent.GetKey() == EKeys::Down)
	{
		SelectionDelta = +1;
	}

	if (SelectionDelta != 0)
	{
		AssetViewPtr->AdjustActiveSelection(SelectionDelta);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}


FReply FRealBandAssetLoader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

void FRealBandAssetLoader::SyncToAssets(const TArray<FAssetData>& AssetDataList)
{
	AssetViewPtr->SyncToLegacy(AssetDataList, TArray<FString>());
}

#undef LOCTEXT_NAMESPACE