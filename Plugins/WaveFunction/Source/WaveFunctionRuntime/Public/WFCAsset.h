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
	void InitSetting();
	void ReFillBrushes();
	void BrushIndexSelectedChange(int32 NewIndex);
	int32 GetBrushIndexSelected();
	void BrushSizeChange(FString ChangeType = "Add");
public:
	FSlateBrush* GetBrushInputByIndex(int32 Index);
	FSlateBrush* GetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void SetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void SaveCurrentOutPutAsTemplate();
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
	class SMyTileItem* LastSelected;
	/**/
private:
	/**/
	TArray<FSlateBrush> BrushesInput;
	TArray<FSlateBrush> BrushesOutput;

	TArray<TArray<int32>> OutputIndexList;
	TArray<TArray<int32>> TemplateIndexList;
	UTexture2D* OutputInitTexture;
	FSlateBrush OutputInitBrush;
	int32 BrushIndexSelected;
	/***/
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void ReSizeBrushesOutput();
	void ReFillOutputIndexList();
};