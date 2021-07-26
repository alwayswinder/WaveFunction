// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCAssetFactory.h"
#include "WFCAsset.h"
#include "ClassViewerFilter.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "ClassViewerModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"


#define LOCTEXT_NAMESPACE "WFCAssetFactory"

class SWFCCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWFCCreateDialog) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;

		ChildSlot
		[
			SNew(SBorder)
			.Visibility(EVisibility::Visible)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
			[
				SNew(SBox)
				.Visibility(EVisibility::Visible)
				.WidthOverride(500.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.FillHeight(1)
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.Content()
						[
							SAssignNew(ResContainer, SVerticalBox)
						]
					]

					// Ok/Cancel buttons
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.Padding(8)
					[
						SNew(SUniformGridPanel)
						.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
						.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
						.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
						+ SUniformGridPanel::Slot(0, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
							.OnClicked(this, &SWFCCreateDialog::OkClicked)
							.Text(LOCTEXT("CreateAnimBlueprintOk", "OK"))
						]
						+ SUniformGridPanel::Slot(1, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
							.OnClicked(this, &SWFCCreateDialog::CancelClicked)
							.Text(LOCTEXT("CreateAnimBlueprintCancel", "Cancel"))
						]
					]
				]
			]
		];
		MakeResPicker();
	}

	/** Sets properties for the supplied AnimBlueprintFactory */
	bool ConfigureProperties(TWeakObjectPtr<UWFCAssetFactory> InWFCFactory)
	{
		WFCFactory = InWFCFactory;

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateWFCResListOptions", "Create WFC Res List"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;

		GEditor->EditorAddModalWindow(Window);
		WFCFactory.Reset();

		return bOkClicked;
	}

private:

	/** Creates the combo menu for the target skeleton */
	void MakeResPicker()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SWFCCreateDialog::OnResSelected);
		//AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SWFCCreateDialog::FilterResBasedOnParentClass);
		AssetPickerConfig.bAllowNullSelection = true;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::Tile;
		//AssetPickerConfig.InitialAssetSelection = TargetRes;
		AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetSelectedReferencerAssets);

		ResContainer->ClearChildren();
		ResContainer->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TargetRes", "Target Res:"))
				.ShadowOffset(FVector2D(1.0f, 1.0f))
			];

		ResContainer->AddSlot()
			[
				ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
			];
	}

	bool FilterResBasedOnParentClass(const FAssetData& AssetData)
	{
		//return !CanCreateAnimBlueprint(AssetData, ResClass.Get());
		return false;
	}

	/** Handler for when a skeleton is selected */
	void OnResSelected(const FAssetData& AssetData)
	{
		TargetRes = GetSelectedReferencerAssets.Execute();
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if (WFCFactory.IsValid())
		{
			WFCFactory->ResBase = TargetRes;
		}

		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** Handler for when cancel is clicked */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UWFCAssetFactory> WFCFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The container for the target skeleton picker*/
	TSharedPtr<SVerticalBox> ResContainer;

	/** The selected skeleton */
	TArray<FAssetData> TargetRes;

	/** True if Ok was clicked */
	bool bOkClicked;

	FGetCurrentSelectionDelegate GetSelectedReferencerAssets;

};

UWFCAssetFactory::UWFCAssetFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SupportedClass = UWFCAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UWFCAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UWFCAsset* WFCObj =  Cast<UWFCAsset>(NewObject<UWFCAsset>(InParent, InClass, InName, Flags));
	if (WFCObj)
	{
		WFCObj->InitResBase(ResBase);
	}
	return WFCObj;
}

bool UWFCAssetFactory::ConfigureProperties()
{
	TSharedPtr<SWFCCreateDialog> Dialog = SNew(SWFCCreateDialog);
	return Dialog->ConfigureProperties(this);
}

#undef LOCTEXT_NAMESPACE
