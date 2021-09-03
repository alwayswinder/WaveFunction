#include "WFCInputProcessor.h"
#include "WFCTileAssetToolkit.h"
#include "WFCParentAsset.h"

static TSharedPtr<FWFCInputProcessor> WFCInputProcessorInstance;

FWFCInputProcessor::~FWFCInputProcessor()
{

}

void FWFCInputProcessor::Create()
{
	WFCInputProcessorInstance = MakeShareable(new FWFCInputProcessor());
	FSlateApplication::Get().RegisterInputPreProcessor(WFCInputProcessorInstance);
}

FWFCInputProcessor& FWFCInputProcessor::Get()
{
	return *WFCInputProcessorInstance;
}

void FWFCInputProcessor::Cleanup()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(WFCInputProcessorInstance);
	}

	WFCInputProcessorInstance.Reset();
}


void FWFCInputProcessor::AddNewTab(TWeakPtr<SDockTab> Tab, UWFCParentAsset* Asset)
{
	if (!TabMap.Contains(Tab))
	{
		TabMap.Add(Tab, Asset);
	}
}

void FWFCInputProcessor::RemoveTab(TWeakPtr<SDockTab> Tab)
{
	if (TabMap.Contains(Tab))
	{
		TabMap.Remove(Tab);
	}
}

void FWFCInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{

}

bool FWFCInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, 
	const FPointerEvent* InGestureEvent)
{
	if (GEditor->bIsSimulatingInEditor || GEditor->PlayWorld != nullptr)
	{
		return false;
	}
	if (SlateApp.IsInitialized())
	{
		TSharedRef<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
		TSharedPtr<SDockTab> ActiveTab = TabManager->GetActiveTab();
		if (ActiveTab.IsValid() && TabMap.Contains(ActiveTab))
		{	
			UWFCParentAsset* AssetEdited = *TabMap.Find(ActiveTab);
			if (!AssetEdited)
			{
				return false;
			}
			if (InWheelEvent.GetWheelDelta() > 0)
			{
				AssetEdited->BrushSizeChange("Add");
			}
			else if(InWheelEvent.GetWheelDelta() < 0)
			{
				AssetEdited->BrushSizeChange("Sub");
			}
			//UE_LOG(LogTemp, Warning, TEXT("Mouse Up %f"), InWheelEvent.GetWheelDelta());
		}
	}
	return false;
}

FWFCInputProcessor::FWFCInputProcessor()
{

}

