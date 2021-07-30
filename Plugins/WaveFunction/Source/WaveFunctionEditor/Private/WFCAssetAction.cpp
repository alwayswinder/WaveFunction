#include "WFCAssetAction.h"
#include "WFCAsset.h"
#include "WFCAssetToolkit.h"
#include "WFCInputProcessor.h"

FWFCAssetAction::FWFCAssetAction(uint32 InAssetCategory)
	: WFCAssetCategory(InAssetCategory)
{
}

uint32 FWFCAssetAction::GetCategories()
{
	return WFCAssetCategory;
}

FText FWFCAssetAction::GetName() const
{
	return FText::FromString(GetSupportedClass()->GetName());
}

UClass* FWFCAssetAction::GetSupportedClass() const
{
	return UWFCAsset::StaticClass();
}

FColor FWFCAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

void FWFCAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost>
	EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Obj = InObjects.CreateConstIterator(); Obj; ++Obj)
	{
		auto NewAsset = Cast<UWFCAsset>(*Obj);
		if (NewAsset)
		{
			TSharedRef<FWFCAssetToolkit> EditorToolkit = MakeShareable(new FWFCAssetToolkit());
			EditorToolkit->Initialize(NewAsset, Mode, EditWithinLevelEditor);
		}
	}
}

