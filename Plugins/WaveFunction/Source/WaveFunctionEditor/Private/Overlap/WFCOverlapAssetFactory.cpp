// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCOverlapAssetFactory.h"
#include "WFCOverlapAsset.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"


#define LOCTEXT_NAMESPACE "WFCAssetOverlapFactory"


UWFCOverlapAssetFactory::UWFCOverlapAssetFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SupportedClass = UWFCOverlapAsset::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UWFCOverlapAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UWFCOverlapAsset* WFCObj = Cast<UWFCOverlapAsset>(NewObject<UWFCOverlapAsset>(InParent, InClass, InName, Flags));
	if (WFCObj)
	{
		WFCObj->InitCreated();
	}
	return WFCObj;
}

#undef LOCTEXT_NAMESPACE
