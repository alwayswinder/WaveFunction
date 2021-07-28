#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"


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
	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	/**/
	void Initialize(class UWFCAsset* InNewAsset, const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>
		EditWithinLevelEditor);
	UWFCAsset* GetWFCAssetEdited() const { return WFCAsset; }
protected:
	TSharedRef<SDockTab> SpawnTab_Input(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Output(const FSpawnTabArgs& Args);
	void RefreshOutputTab();

	TSharedPtr<SVerticalBox> OutputVbx;
	TSharedPtr<SVerticalBox> InputVbx;
	TSharedPtr<FTabManager> TabManager;

	/**/
	void ReFillInputResHbxs();
	void ReFillOutputResHbxs();

private:
	/**/
	UWFCAsset* WFCAsset;
};