#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"

class UWFCAsset;

class SMyButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyButton) {}
	SLATE_ATTRIBUTE(UWFCAsset*, WFCAsset)
	SLATE_ATTRIBUTE(int32, BrushIndex)
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
private:
	TSharedPtr<class SButton> Button;
	TSharedPtr<class SBorder> Border;
	UWFCAsset* WFCAsset;
	int32 BrushIndex;
	bool IsOutput;
	int32 RowIndex;
	int32 ColumnIndex;
};

namespace FWFCAssetToolkitTabs
{
	static const FName AppIdentifier("FWCAssetEditorApp");
	static const FName InputTabID("Input");
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
	TSharedRef<SDockTab> SpawnTab_Output(const FSpawnTabArgs& Args);
	void RefreshTabs();

	TSharedPtr<SVerticalBox> OutputVbx;
	TSharedPtr<SVerticalBox> InputVbx;
	/**/
	void ReFillInputResHbxs();
	void ReFillOutputResHbxs();
private:
	/**/
	UWFCAsset* WFCAsset;
	FDelegateHandle PropertyChangeHandle;
	TSharedPtr<SDockTab> OutputTab;
};