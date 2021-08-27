// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "WFCAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FPropertyChangeEvent)



UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCAsset : public UObject
{
	GENERATED_BODY()
public:
	UWFCAsset();
public:
	void InitResBase(TArray<FAssetData>& InResBase);
	void InitSetting();
	void ReFillBrushes();
	void ReFillInputBrushesWithRot();

	void ReFillSymmetrys();
	void ReFillNeighbors();
	void RemoveNeighborByKey(int32 Key);

	void InputileIndexSelectedChange(int32 NewIndex);
	int32 GetInputTileIndexSelected();

	void BrushSizeChange(FString ChangeType = "Add");
	FSlateBrush* GetBrushInputByIndex(int32 Index);
	FSlateBrush* GetBrushInputByTileIndex(int32 Index);
	FSlateBrush* GetBrushOutputByRowAndCloumns(int32 r, int32 c);
	ETileRot GetOutputTileRotByRowAndCloumns(int32 r, int32 c);

	void SetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void ClearBrushOutput();

	void OnOutputAnalysis(int r, int c);
	void OnOutputGenerate();

	int32 GetTilesNum();
	int32 GetSymmetrysNum();
	int32 GetOutputResultNumByRC(int32 r, int32 c);
	ETileRot TileRotAdd(ETileRot inRot);
	void ReSetIsPaint();
	bool GetIsPaint();
	/**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<UTexture2D*> InputRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<ESymmetry> Symmetrys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TMap<int32, FNeighborInfo> Neighbors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WFCOutput")
	int32 OutputRows = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputColumns = 10;
	int32 BrushSize = 64;
	

	FPropertyChangeEvent PropertyChangeInput;
	FPropertyChangeEvent PropertyChangeOutput;

	class SMyOutputTileItem* LastSelected;
	/**/
	TArray<FTilesInfo> InputTileInfoList;

private:
	/**/
	TArray<FSlateBrush> BrushesInput;
	TArray<FSlateBrush> BrushesOutput;
	TArray<TArray<FTilesInfo>> OutputTileInfoList;
	TArray<TArray<TArray<int32>>> OutputTilesMaybe;

	TArray<TArray<int32>> TemplateIndexList;
	UTexture2D* OutputInitTexture;
	FSlateBrush OutputInitBrush;

	int32 InputTileIndexSelected;

	bool IsPaint = true;
	/***/
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void ReSizeBrushesOutput();
	void ReFillOutputIndexList();
};