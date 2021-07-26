// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCAsset.generated.h"


UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCAsset : public UObject
{
	GENERATED_BODY()
public:
	void InitResBase(TArray<FAssetData>& InResBase);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WFC")
	int32 OutputRows = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
	int32 OutputColumns = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
	TArray<UTexture2D*> ResBase;

};