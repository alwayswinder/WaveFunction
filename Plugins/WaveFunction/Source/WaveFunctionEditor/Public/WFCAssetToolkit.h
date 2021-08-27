#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WFCTypes.h"

class UWFCAsset;
class SScrollBox;

class SMyOutputTileItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyOutputTileItem) {}
	SLATE_ATTRIBUTE(UWFCAsset*, WFCAsset)
	SLATE_ATTRIBUTE(int32, InputTileIndex)
	SLATE_ATTRIBUTE(bool, IsOutput)
	SLATE_ATTRIBUTE(int32, RowIndex)
	SLATE_ATTRIBUTE(int32, ColumnIndex)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	void OnHovered();
	void OnUnHovered();
	void OnPressed();
	void OnRealsed();
	void OnNewBrushSelect();
	FText GetNumofResult()const;
private:
	TSharedPtr<class SButton> Button;
	TSharedPtr<class SBorder> Border;
	UWFCAsset* WFCAsset;
	int32 InputTileIndex;
	bool IsOutput;
	int32 RowIndex;
	int32 ColumnIndex;
	bool IsSelected;
};
class SMyTilesSettingItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyTilesSettingItem) {}
	SLATE_ATTRIBUTE(UWFCAsset*, WFCAsset)
	SLATE_ATTRIBUTE(int32, BrushIndex)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	TArray<TSharedPtr<FString>> ComboBoxFilterOptions;
	int32 SelectedComboBoxFilterOptions;
	void ComboBoxWidgetSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> ComboBoxDetailFilterWidget(TSharedPtr<FString> InItem);
	FText GetSelectedComboBoxDetailFilterTextLabel() const;

private:
	TSharedPtr<class SComboBox<TSharedPtr<FString>>> Combox;
	UWFCAsset* WFCAsset;
	int32 BrushIndex;
};

class SMyNeighborsSettingItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyNeighborsSettingItem) {}
	SLATE_ATTRIBUTE(UWFCAsset*, WFCAsset)
	SLATE_ATTRIBUTE(TSharedPtr<SScrollBox>, ParentSlate)
	SLATE_ATTRIBUTE(int32, Key)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	void Removeneighbor();

private:
	TSharedPtr<SScrollBox> ParentSlate;
	UWFCAsset* WFCAsset;
	int32 Key;

};

namespace FWFCAssetToolkitTabs
{
	static const FName AppIdentifier("FWCAssetEditorApp");
	static const FName InputTabID("Input");
	static const FName TilesSettingTabID("TilesSetting");
	static const FName NeighborsSettingTabID("NeighborsSetting");
	static const FName OutputTabID("Output");
}

class FWFCAssetToolkit : public FAssetEditorToolkit, public FGCObject
{
public:
	FWFCAssetToolkit();
	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool OnRequestClose()override;

	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	/**/
	void Initialize(UWFCAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>
		EditWithinLevelEditor);
	UWFCAsset* GetWFCAssetEdited() const { return WFCAsset; }
protected:
	TSharedRef<SDockTab> SpawnTab_Input(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_TilesSetting(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_NeighborsSetting(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Output(const FSpawnTabArgs& Args);
	void RefreshInputTab();
	void RefreshOutputTab();
	void BrushStateChange();
	void ClearOutput();
	void OutputGenerate();
	FText GetBrushStateText()const;


	TSharedPtr<SVerticalBox> OutputVbx;
	TSharedPtr<SVerticalBox> InputVbx;
	TSharedPtr<SScrollBox> TilesSettingSSC;
	TSharedPtr<SScrollBox> NeighborsSettingSSC;
	TSharedPtr<SButton> BtBrushChange;
	/**/
	void ReFillInputResHbxs();
	void ReFillOutputResHbxs();

	void TilsSettingGenerate();
	void NeighborsSettingGenerate();

	void TilsSettingReGenerate();
	void NeighborsSettingReGenerate();

private:
	/**/
	UWFCAsset* WFCAsset;
	FDelegateHandle PropertyChangeHandleInput;
	FDelegateHandle PropertyChangeHandleOutput;

	TSharedPtr<SDockTab> OutputTab;
};