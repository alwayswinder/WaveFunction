// Copyright Epic Games, Inc. All Rights Reserved.

#include "WaveFunctionEditor.h"
#include "WFCTileAssetAction.h"
#include "WFCInputProcessor.h"

#define LOCTEXT_NAMESPACE "FWaveFunctionEditorModule"

uint32 FWaveFunctionEditorModule::GameAssetCategory;


void FWaveFunctionEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	GameAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("WFCAsset")), LOCTEXT("WFCPluginCategory", "WFCAsset"));

	AssetActions.Add(MakeShareable(new FWFCTileAssetAction(GameAssetCategory)));
	AssetActions.Add(MakeShareable(new FWFCOverlapAssetAction(GameAssetCategory)));

	for (auto Action : AssetActions)
	{
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	}

	FWFCInputProcessor::Create();
}

void FWaveFunctionEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FWFCInputProcessor::Get().Cleanup();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWaveFunctionEditorModule, WaveFunctionEditor)