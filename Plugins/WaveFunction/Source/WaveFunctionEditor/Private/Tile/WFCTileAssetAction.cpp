#include "WFCTileAssetAction.h"
#include "WFCTileAsset.h"
#include "WFCTileAssetToolkit.h"
#include "WFCInputProcessor.h"

FWFCTileAssetAction::FWFCTileAssetAction(uint32 InAssetCategory)
	: WFCAssetCategory(InAssetCategory)
{
}

uint32 FWFCTileAssetAction::GetCategories()
{
	return WFCAssetCategory;
}

FText FWFCTileAssetAction::GetName() const
{
	return FText::FromString(GetSupportedClass()->GetName());
}

UClass* FWFCTileAssetAction::GetSupportedClass() const
{
	return UWFCTileAsset::StaticClass();
}

FColor FWFCTileAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

void FWFCTileAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost>
	EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Obj = InObjects.CreateConstIterator(); Obj; ++Obj)
	{
		auto NewAsset = Cast<UWFCTileAsset>(*Obj);
		if (NewAsset)
		{
			TSharedRef<FWFCAssetToolkit> EditorToolkit = MakeShareable(new FWFCAssetToolkit());
			EditorToolkit->Initialize(NewAsset, Mode, EditWithinLevelEditor);
		}
	}
}

