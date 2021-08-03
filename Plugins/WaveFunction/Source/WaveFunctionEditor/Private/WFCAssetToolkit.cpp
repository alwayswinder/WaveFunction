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

	InTabManager->RegisterTabSpawner(FWFCAssetToolkitTabs::TilesSettingTabID, FOnSpawnTab::CreateSP(this, &FWFCAssetToolkit::SpawnTab_TilesSetting))
		.SetDisplayName(LOCTEXT("TilesSettingTab", "TilesSetting"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FWFCAssetToolkitTabs::NeighborsSettingTabID, FOnSpawnTab::CreateSP(this, &FWFCAssetToolkit::SpawnTab_NeighborsSetting))
		.SetDisplayName(LOCTEXT("NeighborsSettingTab", "NeighborsSetting"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FWFCAssetToolkitTabs::OutputTabID, FOnSpawnTab::CreateSP(this, &FWFCAssetToolkit::SpawnTab_Output))
		.SetDisplayName(LOCTEXT("OutputTab", "Output"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Modes"));

	if (WFCAsset)
	{
		PropertyChangeHandleInput = WFCAsset->PropertyChangeInput.AddRaw(this, &FWFCAssetToolkit::RefreshInputTab);
		PropertyChangeHandleOutput = WFCAsset->PropertyChangeOutput.AddRaw(this, &FWFCAssetToolkit::RefreshOutputTab);
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
	WFCAsset->PropertyChangeInput.Remove(PropertyChangeHandleInput);
	WFCAsset->PropertyChangeOutput.Remove(PropertyChangeHandleOutput);

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
					/*left*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FWFCAssetToolkitTabs::TilesSettingTabID, ETabState::OpenedTab)
					->AddTab(FWFCAssetToolkitTabs::NeighborsSettingTabID, ETabState::OpenedTab)
					->SetForegroundTab(FWFCAssetToolkitTabs::TilesSettingTabID)
				)
				->Split
				(
					/*right*/
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->SetHideTabWell(true)
					->AddTab(FWFCAssetToolkitTabs::OutputTabID, ETabState::OpenedTab)
				)
			)
		);

	InitAssetEditor(InMode, EditWithinLevelEditor, FWFCAssetToolkitTabs::AppIdentifier, WFCLayout, true, true, InNewAsset);
	RegenerateMenusAndToolbars();
	TilsSettingGenerate();
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

TSharedRef<SDockTab> FWFCAssetToolkit::SpawnTab_TilesSetting(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("TilesTab_Title", "TilesSetting"))
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				[
					SNew(SButton)
					.Text(FText::FromString("Generate"))
					.OnPressed(this, &FWFCAssetToolkit::TilsSettingGenerate)
				]		
			] 
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SAssignNew(TilesSettingVbx, SVerticalBox)
			]
		];
}

TSharedRef<SDockTab> FWFCAssetToolkit::SpawnTab_NeighborsSetting(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("NeighborsTab_Title", "NeighborsSetting"))
		[
			SAssignNew(NeighborsSettingVbx, SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				[
					SNew(SButton)
					.Text(FText::FromString("Generate"))
					.OnPressed(this, &FWFCAssetToolkit::NeighborsSettingGenerate)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				[
					SNew(SButton)
					.Text(FText::FromString("Save"))
					.OnPressed(this, &FWFCAssetToolkit::NeighborsSettingSave)
				]
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
					.Text(FText::FromString("Paint"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Remove"))
					//.OnPressed(this, &FWFCAssetToolkit::OnSavePressed)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Clrea"))
					//.OnPressed(this, &FWFCAssetToolkit::OnSavePressed)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Generate"))
					//.OnPressed(this, &FWFCAssetToolkit::OnSavePressed)
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

void FWFCAssetToolkit::RefreshInputTab()
{
	ReFillInputResHbxs();
}

void FWFCAssetToolkit::RefreshOutputTab()
{
	ReFillOutputResHbxs();
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
				int32 BrushIndex =  r * RowEach + c;
				TmpHbx->AddSlot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SBorder)
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						.Padding(0)
						[
							SNew(SMyOutputTileItem)
							.WFCAsset(WFCAsset)
							.IsOutput(false)
							.BrushIndex(BrushIndex)
						]
					];
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
							SNew(SMyOutputTileItem)
							.WFCAsset(WFCAsset)
							.IsOutput(true)
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
		}
	}
}

void FWFCAssetToolkit::TilsSettingGenerate()
{
	TilesSettingVbx->ClearChildren();
	if (WFCAsset)
	{
		WFCAsset->ReFillSymmetrys();
		for (int32 i=0; i<WFCAsset->GetTilesNum(); i++)
		{
			TilesSettingVbx->AddSlot()
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SNew(SMyTilesSettingItem)
					.WFCAsset(WFCAsset)
					.BrushIndex(i)
				];
		}
	}
}

void FWFCAssetToolkit::TilsSettingSave()
{

}

void FWFCAssetToolkit::NeighborsSettingGenerate()
{

}

void FWFCAssetToolkit::NeighborsSettingSave()
{

}

void SMyOutputTileItem::Construct(const FArguments& InArgs)
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
					.OnHovered(this, &SMyOutputTileItem::OnHovered)
					.OnUnhovered(this, &SMyOutputTileItem::OnUnHovered)
					.OnPressed(this, &SMyOutputTileItem::OnPressed)
					.OnReleased(this, &SMyOutputTileItem::OnRealsed)
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
	else
	{
		Brush = WFCAsset->GetBrushInputByIndex(BrushIndex);
		ChildSlot
			[

				SAssignNew(Button, SButton)
				.OnHovered(this, &SMyOutputTileItem::OnHovered)
				.OnUnhovered(this, &SMyOutputTileItem::OnUnHovered)
				.OnPressed(this, &SMyOutputTileItem::OnPressed)
				.OnReleased(this, &SMyOutputTileItem::OnRealsed)
				.ContentPadding(0)
				.Content()
				[
					SNew(SImage)
					.Image(Brush)
				]
			];
	}
	
}

void SMyOutputTileItem::OnHovered()
{
	Button->SetColorAndOpacity(FLinearColor(0.2, 0.2, 1, 1));
}

void SMyOutputTileItem::OnUnHovered()
{
	if (!IsSelected)
	{
		Button->SetColorAndOpacity(FLinearColor::White);
	}
	else
	{
		Button->SetColorAndOpacity(FLinearColor::Green);
	}
}

void SMyOutputTileItem::OnPressed()
{
	Button->SetColorAndOpacity(FLinearColor::Green);
	if (IsOutput)
	{
		WFCAsset->SetBrushOutputByRowAndCloumns(RowIndex, ColumnIndex);
	}
	else
	{
		if (WFCAsset->GetBrushIndexSelected() != BrushIndex)
		{
			WFCAsset->BrushIndexSelectedChange(BrushIndex);
			if (WFCAsset->LastSelected)
			{
				WFCAsset->LastSelected->OnNewBrushSelect();
			}
			WFCAsset->LastSelected = this;
		}	IsSelected = true;
	}
	
}

void SMyOutputTileItem::OnRealsed()
{
	if (IsOutput)
	{
		Button->SetColorAndOpacity(FLinearColor::White);
	}
}

void SMyOutputTileItem::OnNewBrushSelect()
{
	Button->SetColorAndOpacity(FLinearColor::White);
	IsSelected = false;
}



void SMyTilesSettingItem::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	BrushIndex = InArgs._BrushIndex.Get();
	UEnum* SymmetrysEnum = StaticEnum<ESymmetry>();
	for (int32 i=0; i<SymmetrysEnum->NumEnums()-1; i++)
	{
		FString EnumName = StaticEnum<ESymmetry>()->GetNameStringByValue(SymmetrysEnum->GetValueByIndex(i));
		ComboBoxFilterOptions.Add(MakeShared<FString>(EnumName));
	}
	if (WFCAsset && WFCAsset->Symmetrys.IsValidIndex(BrushIndex))
	{
		SelectedComboBoxFilterOptions = static_cast<int32>(WFCAsset->Symmetrys[BrushIndex]);
	}
	else
	{
		SelectedComboBoxFilterOptions = 0;
	}
	ChildSlot
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2)
			[
				SNew(SImage)
				.Image(WFCAsset->GetBrushInputByIndex(BrushIndex))
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(2)
			[
				SAssignNew(Combox, SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&ComboBoxFilterOptions)
				.InitiallySelectedItem(ComboBoxFilterOptions[SelectedComboBoxFilterOptions])
				.OnSelectionChanged(this, &SMyTilesSettingItem::ComboBoxWidgetSelectionChanged)
				.OnGenerateWidget(this, &SMyTilesSettingItem::ComboBoxDetailFilterWidget)
				[
					SNew(STextBlock)
					.Text(this, &SMyTilesSettingItem::GetSelectedComboBoxDetailFilterTextLabel)
				]
			]
		];
}

void SMyTilesSettingItem::ComboBoxWidgetSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	UEnum* SymmetrysEnum = StaticEnum<ESymmetry>();

	for (int32 i = 0; i < ComboBoxFilterOptions.Num(); ++i)
	{
		if (ComboBoxFilterOptions[i] == NewSelection)
		{
			SelectedComboBoxFilterOptions = i;
			if (WFCAsset->Symmetrys.IsValidIndex(BrushIndex))
			{
				WFCAsset->Symmetrys[BrushIndex] = ESymmetry(SymmetrysEnum->GetValueByIndex(i));
			}
			break;
		}
	}
}

TSharedRef<SWidget> SMyTilesSettingItem::ComboBoxDetailFilterWidget(TSharedPtr<FString> InItem)
{
	FString ItemString;
	if (InItem.IsValid())
	{
		ItemString = *InItem;
	}
	return SNew(STextBlock).Text(FText::FromString(*ItemString));
}

FText SMyTilesSettingItem::GetSelectedComboBoxDetailFilterTextLabel() const
{
	int32 ComboBoxDetailFilterOptionsNum = ComboBoxFilterOptions.Num();
	int32 SelectionIndex = SelectedComboBoxFilterOptions < 0 ? 0 
		: (SelectedComboBoxFilterOptions < ComboBoxDetailFilterOptionsNum ? SelectedComboBoxFilterOptions
			: ComboBoxDetailFilterOptionsNum - 1);
	return FText::FromString(*ComboBoxFilterOptions[SelectionIndex]);
}

#undef LOCTEXT_NAMESPACE