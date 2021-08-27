
#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.generated.h"


UENUM(BlueprintType)
enum class ESymmetry : uint8
{
	I = 0,
	X,
	T,
	L,
};

UENUM(BlueprintType)
enum class ETileRot : uint8
{
	nine = 0,
	OneEight,
	TowSeven,
	None,
	//LR,
	//UD,
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FTilesInfo
{
	GENERATED_BODY()
	FTilesInfo() {};
	FTilesInfo(int32 I, ETileRot TR)
	{
		index = I;
		Rot = TR;
	}
	UPROPERTY(EditAnywhere)
		int32 index = -1;
	UPROPERTY(EditAnywhere)
		ETileRot Rot = ETileRot::None;
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FNeighborInfo
{
	GENERATED_BODY()
		FNeighborInfo() {};
	FNeighborInfo(int32 L, int32 R)
	{
		Left.index = L;
		Right.index = R;
	}
	UPROPERTY(EditAnywhere)
		FTilesInfo Left;
	UPROPERTY(EditAnywhere)
		FTilesInfo Right;
};