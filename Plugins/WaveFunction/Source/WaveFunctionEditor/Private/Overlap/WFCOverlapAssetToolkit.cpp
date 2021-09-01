#include "WFCOverlapAssetToolkit.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorStyleSet.h"
#include "WFCOverlapAsset.h"
#include "Widgets\Layout\SScrollBox.h"


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

	InTabManager->RegisterTabSpawner(FWFCOverlapAssetToolkitTabs::OutputTabID, FOnSpawnTab::CreateSP(this, &FWFCOverlapAssetToolkit::SpawnTab_Output))
		.SetDisplayName(LOCTEXT("OverlapOutputTab", "OverlapOutput"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Modes"));
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
					/*right*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
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

TSharedRef<SDockTab> FWFCOverlapAssetToolkit::SpawnTab_Output(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
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
					SNew(SButton)
					.Text(FText::FromString("Fill"))
					.OnPressed(this, &FWFCOverlapAssetToolkit::FillOutput)
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SImage)
				.Image(&(WFCAsset->OutputBrush))
				.RenderTransformPivot((FVector2D(0.5f, 0.5f)))
				.RenderTransform(TransformCast<FSlateRenderTransform>(Concatenate(FShear2D(0, 0),
					FScale2D(WFCAsset->OutputXScale, WFCAsset->OutputYScale), FQuat2D(0))))
			]
		];
}

void FWFCOverlapAssetToolkit::Analyse()
{
	WFCAsset->Analyse();
}

void FWFCOverlapAssetToolkit::ClearOutput()
{
	WFCAsset->ClearOutput();
}

void FWFCOverlapAssetToolkit::FillOutput()
{
	WFCAsset->FillOutput();
}

#undef LOCTEXT_NAMESPACE