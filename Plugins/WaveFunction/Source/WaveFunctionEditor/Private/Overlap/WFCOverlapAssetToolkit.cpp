#include "WFCOverlapAssetToolkit.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorStyleSet.h"
#include "WFCOverlapAsset.h"
#include "Widgets\Layout\SScrollBox.h"
#include "WFCInputProcessor.h"


#define LOCTEXT_NAMESPACE "WFCOverlapEditor"

class SWFCOverlapPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(SWFCOverlapPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	// Pointer back to owning TileMap editor instance (the keeper of state)
	TWeakPtr<class FWFCOverlapAssetToolkit> WFCEditorPtr;
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FWFCOverlapAssetToolkit> InWFCEditor)
	{
		WFCEditorPtr = InWFCEditor;

		SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments()
			.HostCommandList(InWFCEditor->GetToolkitCommands()).HostTabManager(InWFCEditor->GetTabManager()),
			/*bAutoObserve=*/ true, /*bAllowSearch=*/ true);
	}

	// SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override
	{
		return WFCEditorPtr.Pin()->GetWFCAssetEdited();
	}

	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				PropertyEditorWidget
			];
	}
	// End of SSingleObjectDetailsPanel interface
};

FWFCOverlapAssetToolkit::FWFCOverlapAssetToolkit()
{
}

void FWFCOverlapAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_WFCOverlapEditor", "WFC Overlap Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FWFCOverlapAssetToolkitTabs::InputTabID, FOnSpawnTab::CreateSP(this, &FWFCOverlapAssetToolkit::SpawnTab_Input))
		.SetDisplayName(LOCTEXT("OverlapInputTab", "OverlapInput"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FWFCOverlapAssetToolkitTabs::TilesTabID, FOnSpawnTab::CreateSP(this, &FWFCOverlapAssetToolkit::SpawnTab_Tiles))
		.SetDisplayName(LOCTEXT("OverlapTilesTab", "OverlapTiles"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FWFCOverlapAssetToolkitTabs::NeighborsTabID, FOnSpawnTab::CreateSP(this, &FWFCOverlapAssetToolkit::SpawnTab_Neighbors))
		.SetDisplayName(LOCTEXT("OverlapNeighborsTab", "OverlapNeighbors"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
	InTabManager->RegisterTabSpawner(FWFCOverlapAssetToolkitTabs::OutputTabID, FOnSpawnTab::CreateSP(this, &FWFCOverlapAssetToolkit::SpawnTab_Output))
		.SetDisplayName(LOCTEXT("OverlapOutputTab", "OverlapOutput"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Modes"));

	if (WFCAsset)
	{
		//PropertyChangeHandleInput = WFCAsset->PropertyChangeInput.AddRaw(this, &FWFCAssetToolkit::RefreshInputTab);
		PropertyChangeHandleOutput = WFCAsset->PropertyChangeOutput.AddRaw(this, &FWFCOverlapAssetToolkit::RefreshOutputTab);
	}
}

void FWFCOverlapAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FName FWFCOverlapAssetToolkit::GetToolkitFName() const
{
	return FName("WFCOverlapAssetToolkit");
}

FText FWFCOverlapAssetToolkit::GetBaseToolkitName() const
{
	return FText::FromString(TEXT("WFCOverlapAssetBaseToolkit"));
}

FString FWFCOverlapAssetToolkit::GetWorldCentricTabPrefix() const
{
	return FString(TEXT("WFCOverlapAssetEditorPrefix"));
}

FLinearColor FWFCOverlapAssetToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Blue;
}

bool FWFCOverlapAssetToolkit::OnRequestClose()
{
	FWFCInputProcessor::Get().RemoveTab(OutputTab);
	WFCAsset->PropertyChangeOutput.Remove(PropertyChangeHandleOutput);
	return  true;
}

void FWFCOverlapAssetToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(WFCAsset);
}

void FWFCOverlapAssetToolkit::Initialize(class UWFCOverlapAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(InNewAsset, this);
	WFCAsset = InNewAsset;

	const TSharedRef<FTabManager::FLayout> WFCLayout =
		FTabManager::NewLayout("Standalone_WFCOverlapEditor_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					/*left*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FWFCOverlapAssetToolkitTabs::InputTabID, ETabState::OpenedTab)
				)
				->Split
				(
					/*left*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FWFCOverlapAssetToolkitTabs::TilesTabID, ETabState::OpenedTab)
					->AddTab(FWFCOverlapAssetToolkitTabs::NeighborsTabID, ETabState::OpenedTab)
					->SetForegroundTab(FWFCOverlapAssetToolkitTabs::TilesTabID)
				)
				->Split
				(
					/*right*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->SetHideTabWell(true)
					->AddTab(FWFCOverlapAssetToolkitTabs::OutputTabID, ETabState::OpenedTab)
				)
			)
		);

	InitAssetEditor(InMode, EditWithinLevelEditor, FWFCOverlapAssetToolkitTabs::AppIdentifier, WFCLayout, true, true, InNewAsset);
	RegenerateMenusAndToolbars();
}


TSharedRef<SDockTab> FWFCOverlapAssetToolkit::SpawnTab_Input(const FSpawnTabArgs& Args)
{
	TSharedPtr<FWFCOverlapAssetToolkit> WFCEditorPtr = SharedThis(this);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			SNew(SWFCOverlapPropertiesTabBody, WFCEditorPtr)
		];
}

TSharedRef<SDockTab> FWFCOverlapAssetToolkit::SpawnTab_Tiles(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("Tab_Tiles", "Tiles"))
		[
			SAssignNew(TilesSSC, SScrollBox)
		];
}

TSharedRef<SDockTab> FWFCOverlapAssetToolkit::SpawnTab_Neighbors(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("Tab_Neighbors", "Neighbors"))
		[
			SAssignNew(NeighborsSSC, SScrollBox)
		];
}

TSharedRef<SDockTab> FWFCOverlapAssetToolkit::SpawnTab_Output(const FSpawnTabArgs& Args)
{
	OutputVbx = SNew(SVerticalBox);

	RefreshOutputTab();

	OutputTab =  SNew(SDockTab)
		.ContentPadding(20)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Analyse"))
					.OnPressed(this, &FWFCOverlapAssetToolkit::Analyse)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Clear"))
					.OnPressed(this, &FWFCOverlapAssetToolkit::ClearOutput)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SAssignNew(FillButton, SButton)
					.Text(FText::FromString("Fill"))
					.Visibility(EVisibility::Hidden)
					.OnPressed(this, &FWFCOverlapAssetToolkit::FillOutput)
				]
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				OutputVbx.ToSharedRef()
			]
		];
	FWFCInputProcessor::Get().AddNewTab(OutputTab, WFCAsset);

	return OutputTab.ToSharedRef();

}

void FWFCOverlapAssetToolkit::Analyse()
{
	WFCAsset->Analyse();
	FillButton->SetVisibility(EVisibility::Visible);

	FillTilesAndNeighborsTab();
}

void FWFCOverlapAssetToolkit::ClearOutput()
{
	WFCAsset->ClearOutput();
}

void FWFCOverlapAssetToolkit::FillOutput()
{
	WFCAsset->FillOutput();
}

void FWFCOverlapAssetToolkit::RefreshOutputTab()
{
	if (WFCAsset && OutputVbx.IsValid())
	{
		OutputVbx->ClearChildren();
		/*for (int32 r = 0; r < WFCAsset->OutputRow; r++)
		{
			TSharedPtr<SHorizontalBox> TmpHbx = SNew(SHorizontalBox);

			for (int32 c = 0; c < WFCAsset->OutputColumn; c++)
			{
				TmpHbx->AddSlot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0)
					[
						SNew(SBorder)
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						.Padding(0)
						[
							SNew(SMyOutputOverlapItem)
							.WFCAsset(WFCAsset)
							.RowIndex(r)
							.ColumnIndex(c)
						]
					];
			}
			OutputVbx->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(0)
				[
					TmpHbx.ToSharedRef()
				];
		}*/
		OutputVbx->AddSlot()
			.FillHeight(1.0)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SImage)
				.Image(&(WFCAsset->OutputBrush))
				.RenderTransformPivot((FVector2D(0.5f, 0.5f)))
				.RenderTransform(TransformCast<FSlateRenderTransform>(Concatenate(FShear2D(0, 0),
					FScale2D(WFCAsset->OutputScale, WFCAsset->OutputScale), FQuat2D(0))))
			];
	}
}



void FWFCOverlapAssetToolkit::FillTilesAndNeighborsTab()
{
	TilesSSC->ClearChildren();
	if (WFCAsset->AllOverlapTiles.Num())
	{
		for (int i=0; i<WFCAsset->AllOverlapTiles.Num(); i++)
		{
			TilesSSC->AddSlot()
				.HAlign(HAlign_Left)
				[
					SNew(SMyOutputOverlapTile)
					.TileIndex(i)
					.WFCAsset(WFCAsset)
				];
			TilesSSC->AddSlot()
				.HAlign(HAlign_Fill)
				[
					SNew(SBorder)
				];
		}
	}
}

void SMyOutputOverlapItem::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	RowIndex = InArgs._RowIndex.Get();
	ColumnIndex = InArgs._ColumnIndex.Get();

	ChildSlot
		[
			SNew(SBorder)
			.Padding(1)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.RenderTransformPivot((FVector2D(0.5f, 0.5f)))
			.RenderTransform(TransformCast<FSlateRenderTransform>(Concatenate(FShear2D(0, 0),
				FScale2D(0.5, 0.5), FQuat2D(0))))
			[
				SNew(STextBlock)
				.Text(this, &SMyOutputOverlapItem::GetNumofResult)
			]
		];
}

FText SMyOutputOverlapItem::GetNumofResult() const
{
	int num = -1;
	if (WFCAsset->OutputTilesMaybe.Num() > 0)
	{
		num = WFCAsset->OutputTilesMaybe[RowIndex][ColumnIndex].Num();
	}
	return FText::FromString(FString::FromInt(num));
}



void SMyOutputOverlapTile::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	TileIndex = InArgs._TileIndex.Get();

	TSharedPtr<SVerticalBox> TilesVbx = SNew(SVerticalBox);

	FButtonStyle* MyButtonStyle = new FButtonStyle();
	const FButtonStyle& NoramStyle = FCoreStyle::Get().GetWidgetStyle< FButtonStyle >("Button");

	MyButtonStyle->SetNormal(NoramStyle.Normal);
	MyButtonStyle->SetHovered(NoramStyle.Hovered);
	MyButtonStyle->SetPressed(NoramStyle.Pressed);
	MyButtonStyle->SetDisabled(NoramStyle.Disabled);
	MyButtonStyle->SetNormalPadding(FMargin(0));
	MyButtonStyle->SetPressedPadding(FMargin(0));

	MyButtonStyle->Normal.ImageSize = FVector2D(24, 24);


	for (int c=0; c <WFCAsset->AnalyseSize; c++)
	{
		TSharedPtr<SHorizontalBox> TilesHbxTmp = SNew(SHorizontalBox);
		for (int r=0; r<WFCAsset->AnalyseSize; r++)
		{
			FColor tmp;
			tmp.R = WFCAsset->AllOverlapTiles[TileIndex].Data[c][r].R;
			tmp.G = WFCAsset->AllOverlapTiles[TileIndex].Data[c][r].G;
			tmp.B = WFCAsset->AllOverlapTiles[TileIndex].Data[c][r].B;
			tmp.A = WFCAsset->AllOverlapTiles[TileIndex].Data[c][r].A;

			TilesHbxTmp->AddSlot()
				.Padding(0)
				[
					SNew(SBorder)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.ColorAndOpacity(tmp)
					.Padding(0)
					[
						SNew(SButton)
						.ButtonStyle(MyButtonStyle)
					]
				];
		}
		TilesVbx->AddSlot()
			.Padding(0)
			[
				TilesHbxTmp.ToSharedRef()
			];
	}
	ChildSlot
		[
			TilesVbx.ToSharedRef()
		];

}
#undef LOCTEXT_NAMESPACE