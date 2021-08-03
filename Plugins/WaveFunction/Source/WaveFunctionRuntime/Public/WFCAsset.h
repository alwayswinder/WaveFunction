// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FPropertyChangeEvent)

UENUM(BlueprintType)
enum class ESymmetry : uint8
{
	I = 0,
	X,
	T,
	L,
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FTilesInfo
{
	GENERATED_BODY()
	UPROPERTY()
	int32 index = 0;
	UPROPERTY()
	ESymmetry Symmentry = ESymmetry::I;
};

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
	void ReFillSymmetrys();
	void BrushIndexSelectedChange(int32 NewIndex);
	int32 GetBrushIndexSelected();
	void BrushSizeChange(FString ChangeType = "Add");
	FSlateBrush* GetBrushInputByIndex(int32 Index);
	FSlateBrush* GetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void SetBrushOutputByRowAndCloumns(int32 r, int32 c);
	void SaveCurrentOutPutAsTemplate();
	int32 GetTilesNum();
	/**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<UTexture2D*> InputRes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<ESymmetry> Symmetrys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	TArray<FTilesInfo> Neighbors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WFCOutput")
	int32 OutputRows = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputColumns = 10;
	int32 BrushSize = 64;
	

	FPropertyChangeEvent PropertyChangeInput;
	FPropertyChangeEvent PropertyChangeOutput;

	class SMyOutputTileItem* LastSelected;
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