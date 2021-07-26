#include "WFCAssetToolkit.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorStyleSet.h"


#define LOCTEXT_NAMESPACE "WFCEditor"


class SWFCPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(SWFCPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	// Pointer back to owning TileMap editor instance (the keeper of state)
	TWeakPtr<class FWFCAssetToolkit> WFCEditorPtr;
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FWFCAssetToolkit> InWFCEditor)
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

FWFCAssetToolkit::FWFCAssetToolkit()
{

}

void FWFCAssetToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_WFCEditor", "WFC Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FWFCAssetToolkitTabs::InputTabID, FOnSpawnTab::CreateSP(this, &FWFCAssetToolkit::SpawnTab_Input))
		.SetDisplayName(LOCTEXT("InputTab", "Input"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FWFCAssetToolkitTabs::OutputTabID, FOnSpawnTab::CreateSP(this, &FWFCAssetToolkit::SpawnTab_Output))
		.SetDisplayName(LOCTEXT("OutputTab", "Output"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Modes"));
}

void FWFCAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

FName FWFCAssetToolkit::GetToolkitFName() const
{
	return FName("WFCAssetToolkit");
}

FText FWFCAssetToolkit::GetBaseToolkitName() const
{
	return FText::FromString(TEXT("WFCAssetBaseToolkit"));
}

FString FWFCAssetToolkit::GetWorldCentricTabPrefix() const
{
	return FString(TEXT("WFCAssetEditorPrefix"));
}

FLinearColor FWFCAssetToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Green;
}

void FWFCAssetToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(WFCAsset);
}

void FWFCAssetToolkit::Initialize(class UWFCAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(InNewAsset, this);
	WFCAsset = InNewAsset;

	const TSharedRef<FTabManager::FLayout> WFCLayout = 
		FTabManager::NewLayout("Standalone_WFCEditor_Layout_v1")
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
					->SetSizeCoefficient(0.3f)
					->SetHideTabWell(true)
					->AddTab(FWFCAssetToolkitTabs::InputTabID, ETabState::OpenedTab)
				)
				->Split
				(
					/*right*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FWFCAssetToolkitTabs::OutputTabID, ETabState::OpenedTab)
				)
			)
		);

	InitAssetEditor(InMode, EditWithinLevelEditor, FWFCAssetToolkitTabs::AppIdentifier, WFCLayout, true, true, InNewAsset);
	RegenerateMenusAndToolbars();
}

TSharedRef<SDockTab> FWFCAssetToolkit::SpawnTab_Input(const FSpawnTabArgs& Args)
{
	TSharedPtr<FWFCAssetToolkit> WFCEditorPtr = SharedThis(this);
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			SNew(SWFCPropertiesTabBody, WFCEditorPtr)
		];
}

TSharedRef<SDockTab> FWFCAssetToolkit::SpawnTab_Output(const FSpawnTabArgs& Args)
{
	OutputVbx = SNew(SVerticalBox);

	if (WFCAsset)
	{
		for (int i=0; i<WFCAsset->OutputRows; i++)
		{
			TSharedPtr<SHorizontalBox> OutputHbx = SNew(SHorizontalBox);
			for (int j=0; j<WFCAsset->OutputColumns; j++)
			{
				OutputHbx->AddSlot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
					];
			}
			OutputVbx->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					OutputHbx.ToSharedRef()
				];
		}
	}
	return SNew(SDockTab)
		[
			OutputVbx.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
