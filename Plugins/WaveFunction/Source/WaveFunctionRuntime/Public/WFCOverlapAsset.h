// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "WFCOverlapAsset.generated.h"


UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCOverlapAsset : public UObject
{
	GENERATED_BODY()

	UWFCOverlapAsset();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	UTexture2D* InputImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCInput")
	int32 AnalyseSize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputRow = 128;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFCOutput")
	int32 OutputColumn = 128;

	/*interface*/
	void Analyse();
	void ClearOutput();
	void FillOutput();
	/**/
	FSlateBrush OutputBrush;
	UTexture2D* OutputTexture;
	float OutputXScale = 1.0f;
	float OutputYScale = 1.0f;
protected:
	void WriteToTexture();
private:
};