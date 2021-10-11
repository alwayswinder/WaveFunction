// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "WFCParentAsset.h"
#include "WFCTileAsset.generated.h"


UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCTileAsset : public UWFCParentAsset
{
	GENERATED_BODY()
public:
	UWFCTileAsset();
public:
	void InitResBase(TArray<FAssetData>& InResBase);
	void InitSetting();
	void ReFillBrushes();
	void ReFillBrushesMask();

	void ReFillInputBrushesWithRot();

	void ReFillSymmetrysAndFrequencys();
	void ReFillNeighbors();
	void RemoveNeighborByKey(int32 Key);
	void ReFillNeighborLRAndUD();

	void InputileIndexSelectedChange(int32 NewIndex);
	int32 GetInputTileIndexSelected();

	virtual void BrushSizeChange(FString ChangeType = "Add") override;
	FSlateBrush* GetBrushInputByIndex(int32 Index);
	FSlateBrush* GetBrushInputMaskByIndex(int32 Index);

	FSlateBrush* GetBrushInputByTileIndex(int32 Index);
	FSlateBrush* GetBrushOutputByRowAndCloumns(int32 r, int32 c);
	ETileRot GetOutputTileRotByRowAndCloumns(int32 r, int32 c);

	void SetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void ClearBrushOutput();
	void SaveTileInfoOutput();

	void OnOutputAnalysis(int r, int c);
	void OnOutputGenerate();
	void OnOutputFill();
	void SaveRoadInfo();

	int32 GetTilesNum();
	int32 GetSymmetrysNum();
	int32 GetOutputResultNumByRC(int32 r, int32 c);

	void ReSetIsPaint();
	bool GetIsPaint();
	/**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<UTexture2D*> InputRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<UTexture2D*> InputMaskRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<ESymmetry> Symmetrys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<float> Frequencys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TMap<int32, FNeighborInfo> Neighbors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<FNeighborInfo> NeighborsLR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<FNeighborInfo> NeighborsUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WFCOutput")
	int32 OutputRows = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputColumns = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 TileSize = 64;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 BlurOffSet = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 RandomRate = 30;
	UPROPERTY()
	int32 BrushSize = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	TArray<FTilesInfo> ResultListSave;

	class SMyOutputTileItem* LastSelected;
	/**/
	TArray<FTilesInfo> InputTileInfoList;
	TArray<bool> SaveAsTexture;


private:
	/**/
	TArray<FSlateBrush> BrushesInput;
	TArray<FSlateBrush> BrushesInputMask;

	TArray<FSlateBrush> BrushesOutput;
	TArray<TArray<FTilesInfo>> OutputTileInfoList;
	TArray<TArray<TArray<FTilesInfo>>> OutputTilesMaybe;
	TArray<int32> OutputTimesApear;
	TArray<float> OutputFresuqnceShould;
	TArray<TArray<TArray<FColor>>> InputMaskInfo;

	int32 RowNext;
	int32 ColumnNext;
	float SumFrequency;

	TArray<TArray<int32>> TemplateIndexList;
	UTexture2D* OutputInitTexture;
	FSlateBrush OutputInitBrush;

	int32 InputTileIndexSelected;

	bool IsPaint = true;
	bool IsAllFilled = false;
	/***/
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void ReSizeBrushesOutput();
	void ReFillOutputIndexList();

	FTilesInfo TileRot180(FTilesInfo InTile);
	FTilesInfo TileRot90(FTilesInfo InTile);
	void ReadPixelsFromTexture();
	TArray<FColor> GetPixelsFromInfoSaved(int32 Index, int32 InY, ETileRot InRot);
};