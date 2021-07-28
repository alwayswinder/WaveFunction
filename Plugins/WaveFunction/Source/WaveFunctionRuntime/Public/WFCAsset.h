// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FPropertyChangeEvent)

UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCAsset : public UObject
{
	GENERATED_BODY()
public:
	UWFCAsset();

	void InitResBase(TArray<FAssetData>& InResBase);
public:
	FSlateBrush* GetBrushByIndex(int32 Index);
	FSlateBrush* GetBrushByRowAndCloumns(int32 r, int32 c);
	void ReFillBrushes();
	/**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WFC")
	int32 OutputRows = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
	int32 OutputColumns = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
	int32 BrushSize = 64;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC")
	TArray<UTexture2D*> InputRes;

	FPropertyChangeEvent PropertyChange; 
	/**/
private:
	/**/
	TArray<FSlateBrush> Brushes;
	TArray<TArray<int32>> OutputIndexList;
	UTexture2D* OutputInitTexture;
	FSlateBrush OutputInitBrush;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};