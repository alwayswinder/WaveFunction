#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WFCTypes.h"

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

	/*widget event*/
	void Analyse();
	void ClearOutput();
	void FillOutput();
private:
	UWFCOverlapAsset* WFCAsset;
};