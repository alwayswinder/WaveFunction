// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"

class FWFCAssetAction : public FAssetTypeActions_Base
{
public:
	FWFCAssetAction(uint32 InAssetCategory);

	virtual uint32 GetCategories()override;
	virtual FText GetName()const override;
	virtual UClass* GetSupportedClass()const override;
	virtual FColor GetTypeColor()const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost>
		EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
private:
	uint32 WFCAssetCategory;
};