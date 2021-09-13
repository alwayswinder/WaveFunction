// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "UObject/GCObject.h"
#include "WFCParentAsset.h"
#include "WFCOverlapAsset.generated.h"


UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCOverlapAsset : public UWFCParentAsset
{
	GENERATED_BODY()

	UWFCOverlapAsset();

public:
	virtual void BrushSizeChange(FString ChangeType /* = "Add" */)override;

	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	UTexture2D* InputImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	int32 AnalyseSize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputRow = 128;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputColumn = 128;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 StepOnce = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	bool UseRotAndReflect = false;
	UPROPERTY()
	TArray<FPixelInfo> PixelsOutput;
	UPROPERTY()
	TArray<FPixelInfo> PixelInput;
	int32 Test = 0;
	/*interface*/
	void Analyse();
	void ClearOutput();
	void FillOutput();
	void InitCreated();
	/**/
	UPROPERTY()
	FSlateBrush OutputBrush;
	UPROPERTY()
	UTexture2D* OutputTexture;
	UPROPERTY()
	float OutputScale = 1.0f;

public:
	void WriteToTexture();
	void ReadFromTexture();
	void OnAnalyseStep(int R, int C);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	TArray<FOverlapTileInfo> AllOverlapTiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	TArray<FOverlapNeighborInfo> AllNeighborsLR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	TArray<FOverlapNeighborInfo> AllNeighborsUd;

	/**/
	TArray<TArray<FPixelInfo>> PixelRC;
	TArray<TArray<int32>> OutputTileIndex;
	TArray<TArray<TArray<int32>>> OutputTilesMaybe;

	/**/
	FOverlapTileInfo FindTileFormArray(int r, int c,int rMax, int cMax, TArray<TArray<FPixelInfo>>& PixelArray);
	int32 CheckAndAddTileInAll(FOverlapTileInfo NewTile);
	void AddNeighBorsLR(int32 L, int32 R);
	void AddNeighBorsUD(int32 U, int32 D);

private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

};