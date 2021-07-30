#include "WFCAssetToolkit.h"
#include "SSingleObjectDetailsPanel.h"
#include "EditorStyleSet.h"
#include "WFCAsset.h"
#include "Slate\SMeshWidget.h"
#include "Slate\SlateVectorArtData.h"
#include "Brushes\SlateBoxBrush.h"
#include "Widgets\Layout\SScrollBox.h"
#include "WFCInputProcessor.h"

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

	if (WFCAsset)
	{
		PropertyChangeHandle = WFCAsset->PropertyChange.AddRaw(this, &FWFCAssetToolkit::RefreshTabs);
	}
}

void FWFCAssetToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
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

bool FWFCAssetToolkit::OnRequestClose()
{
	WFCAsset->PropertyChange.Remove(PropertyChangeHandle);
	FWFCInputProcessor::Get().RemoveTab(OutputTab);
	return  true;
}

void FWFCAssetToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(WFCAsset);
}

void FWFCAssetToolkit::Initialize(class UWFCAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(InNewAsset, this);
	WFCAsset = InNewAsset;
	/*没地方调用了，构造函数WFC->InputRes还没数据*/
	WFCAsset->InitSetting();

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
	InputVbx = SNew(SVerticalBox);
	ReFillInputResHbxs();

	/*InputVbx->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(2,5,2,5)
		[
			SNew(SWFCPropertiesTabBody, WFCEditorPtr)
		];*/
	
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DetailsTab_Title", "Details"))
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				InputVbx.ToSharedRef()
			]
			+SScrollBox::Slot()
			.Padding(2, 5, 2, 5)
			[
				SNew(SWFCPropertiesTabBody, WFCEditorPtr)
			]
		];
}

TSharedRef<SDockTab> FWFCAssetToolkit::SpawnTab_Output(const FSpawnTabArgs& Args)
{
	OutputVbx = SNew(SVerticalBox);

	ReFillOutputResHbxs();
	OutputTab = SNew(SDockTab)
		.ContentPadding(20)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
				]
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal)
				+SScrollBox::Slot()
				[
					SNew(SScrollBox)
					.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft)
					+SScrollBox::Slot()
					[
						OutputVbx.ToSharedRef()
					]
				]
			]
		];
	OutputTab->SetTag("OutputTab");
	FWFCInputProcessor::Get().AddNewTab(OutputTab, WFCAsset);
	return OutputTab.ToSharedRef();
}

void FWFCAssetToolkit::RefreshTabs()
{
	ReFillInputResHbxs();
	ReFillOutputResHbxs();
	/*if (TabManager)
	{
		TSharedPtr<SDockTab> OutputTab = TabManager->FindExistingLiveTab(FWFCAssetToolkitTabs::OutputTabID);
		if (OutputTab.IsValid())
		{
			OutputTab->FlashTab();
		}
	}*/
}

void FWFCAssetToolkit::ReFillInputResHbxs()
{
	InputVbx->ClearChildren();
	if (WFCAsset)
	{
		int32 RowEach = 8;
		int32 Rows = (WFCAsset->InputRes.Num() - 1) / RowEach;

		for (int32 r=0; r<=Rows; r++)
		{
			TSharedPtr<SHorizontalBox> TmpHbx = SNew(SHorizontalBox);

			for (int32 c = 0; c < RowEach && r * Rows + c < WFCAsset->InputRes.Num(); c++)
			{
				FSlateBrush* Brush = WFCAsset->GetBrushInputByIndex(r * RowEach + c);
				if (Brush)
				{
					TmpHbx->AddSlot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							.ContentPadding(0)
							.Content()
							[
								SNew(SImage)
								.Image(Brush)
							]
						];
				}
			}
			InputVbx->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					TmpHbx.ToSharedRef()
				];
		}
	}
}

void FWFCAssetToolkit::ReFillOutputResHbxs()
{
	OutputVbx->ClearChildren();
	if (WFCAsset)
	{
		for (int32 r = 0; r < WFCAsset->OutputRows; r++)
		{
			TSharedPtr<SHorizontalBox> TmpHbx = SNew(SHorizontalBox);
			
			for (int32 c = 0; c < WFCAsset->OutputColumns; c++)
			{
				FSlateBrush* Brush = WFCAsset->GetBrushOutputByRowAndCloumns(r, c);
				if (Brush)
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
								SNew(SMyButton)
								.WFCAsset(WFCAsset)
								.IsOutput(true)
								.RowIndex(r)
								.ColumnIndex(c)
							]
						];
				}
			}
			OutputVbx->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				.Padding(0)
				[
					TmpHbx.ToSharedRef()
				];
		}
	}
}


void SMyButton::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	BrushIndex = InArgs._BrushIndex.Get();
	IsOutput = InArgs._IsOutput.Get();
	RowIndex = InArgs._RowIndex.Get();
	ColumnIndex = InArgs._ColumnIndex.Get();
	FSlateBrush* Brush;
	if (IsOutput)
	{
		Brush = WFCAsset->GetBrushOutputByRowAndCloumns(RowIndex, ColumnIndex);
	}
	else
	{
		Brush = WFCAsset->GetBrushInputByIndex(BrushIndex);
	}
	FButtonStyle* MyButtonStyle = new FButtonStyle();
	const FButtonStyle& NoramStyle = FCoreStyle::Get().GetWidgetStyle< FButtonStyle >("Button");
	MyButtonStyle->SetNormal(NoramStyle.Normal);
	MyButtonStyle->SetHovered(NoramStyle.Hovered);
	MyButtonStyle->SetPressed(NoramStyle.Pressed);
	MyButtonStyle->SetDisabled(NoramStyle.Disabled);
	MyButtonStyle->SetNormalPadding(FMargin(0));
	MyButtonStyle->SetPressedPadding(FMargin(0));

	ChildSlot
		[
			SAssignNew(Border, SBorder)
			.Padding(1)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(Button, SButton)
				.OnHovered(this, &SMyButton::OnHovered)
				.OnUnhovered(this, &SMyButton::OnUnHovered)
				.OnPressed(this, &SMyButton::OnPressed)
				.OnReleased(this, &SMyButton::OnRealsed)
				.ButtonStyle(MyButtonStyle)
				.ContentPadding(0)
				.Content()
				[
					SNew(SImage)
					.Image(Brush)
				]
			]
		];
}

void SMyButton::OnHovered()
{
	Button->SetColorAndOpacity(FLinearColor(0.2, 0.2, 1, 1));
}

void SMyButton::OnUnHovered()
{
	Button->SetColorAndOpacity(FLinearColor::White);
}

void SMyButton::OnPressed()
{
	Button->SetColorAndOpacity(FLinearColor::Green);
}

void SMyButton::OnRealsed()
{
	Button->SetColorAndOpacity(FLinearColor::White);
}

#undef LOCTEXT_NAMESPACE