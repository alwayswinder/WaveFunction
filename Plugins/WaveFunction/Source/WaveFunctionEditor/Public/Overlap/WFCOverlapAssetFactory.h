// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories\Factory.h"
#include "EditorReimportHandler.h"
#include "WFCOverlapAssetFactory.generated.h"

/**
 *
 */
UCLASS()
class  UWFCOverlapAssetFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	/**/
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn)override;
public:
	TArray<FAssetData> ResBase;
};
