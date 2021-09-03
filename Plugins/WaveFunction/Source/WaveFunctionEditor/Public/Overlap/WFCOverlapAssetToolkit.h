#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WFCTypes.h"

class UWFCOverlapAsset;



class SMyOutputOverlapItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyOutputOverlapItem) {}
	SLATE_ATTRIBUTE(UWFCOverlapAsset*, WFCAsset)
	SLATE_ATTRIBUTE(int32, RowIndex)
	SLATE_ATTRIBUTE(int32, ColumnIndex)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	FText GetNumofResult()const;
private:
	UWFCOverlapAsset* WFCAsset;
	int32 RowIndex;
	int32 ColumnIndex;
};

class UWFCOverlapAsset;
namespace FWFCOverlapAssetToolkitTabs
{
	static const FName AppIdentifier("FWFCOverlapAssetEditorApp");
	static const FName InputTabID("OverlapInput");
	static const FName OutputTabID("OverlapOutput");
}

class FWFCOverlapAssetToolkit : public FAssetEditorToolkit, public FGCObject
{
public:
	FWFCOverlapAssetToolkit();
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
	void Initialize(UWFCOverlapAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>
		EditWithinLevelEditor);
	UWFCOverlapAsset* GetWFCAssetEdited() const { return WFCAsset; }

protected:
	TSharedRef<SDockTab> SpawnTab_Input(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Output(const FSpawnTabArgs& Args);
	FDelegateHandle PropertyChangeHandleOutput;
	TSharedPtr<SVerticalBox> OutputVbx;
	TSharedPtr<SButton> FillButton;

	/*widget event*/
	void Analyse();
	void ClearOutput();
	void FillOutput();
	void RefreshOutputTab();
private:
	UWFCOverlapAsset* WFCAsset;
	TSharedPtr<SDockTab> OutputTab;

};