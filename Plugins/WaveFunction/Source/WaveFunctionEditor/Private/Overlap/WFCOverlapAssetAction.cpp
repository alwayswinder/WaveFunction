#include "WFCOverlapAssetAction.h"
#include "WFCOverlapAsset.h"
#include "WFCOverlapAssetToolkit.h"

FWFCOverlapAssetAction::FWFCOverlapAssetAction(uint32 InAssetCategory)
	: WFCAssetCategory(InAssetCategory)
{
}

uint32 FWFCOverlapAssetAction::GetCategories()
{
	return WFCAssetCategory;
}

FText FWFCOverlapAssetAction::GetName() const
{
	return FText::FromString(GetSupportedClass()->GetName());
}

UClass* FWFCOverlapAssetAction::GetSupportedClass() const
{
	return UWFCOverlapAsset::StaticClass();
}

FColor FWFCOverlapAssetAction::GetTypeColor() const
{
	return FColor::Blue;
}

void FWFCOverlapAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost>
	EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Obj = InObjects.CreateConstIterator(); Obj; ++Obj)
	{
		auto NewAsset = Cast<UWFCOverlapAsset>(*Obj);
		if (NewAsset)
		{
			TSharedRef<FWFCOverlapAssetToolkit> EditorToolkit = MakeShareable(new FWFCOverlapAssetToolkit());
			EditorToolkit->Initialize(NewAsset, Mode, EditWithinLevelEditor);
		}
	}
}

