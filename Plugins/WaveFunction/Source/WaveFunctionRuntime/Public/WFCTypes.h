
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

USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FPixelInfo
{
	GENERATED_BODY()
	FPixelInfo() {};
	FPixelInfo(int32 r, int32 g,int32 b, int32 a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
	UPROPERTY(EditAnywhere)
	uint8 R;
	UPROPERTY(EditAnywhere)
	uint8 G;
	UPROPERTY(EditAnywhere)
	uint8 B;
	UPROPERTY(EditAnywhere)
	uint8 A;
};
USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FOverlapTileInfo
{
	GENERATED_BODY()
	FOverlapTileInfo() {};
	FOverlapTileInfo(int32 Insize)
	{
		TileSize = Insize;
		for (int r =0; r<TileSize;r++)
		{
			TArray<FPixelInfo> Tmp;
			for (int c=0; c< TileSize; c++)
			{
				Tmp.Add(FPixelInfo(255,255,255,255));
			}
			Data.Add(Tmp);
		}
	}
	UPROPERTY()
	int32 TileSize = 3;
	TArray<TArray<FPixelInfo>> Data;
	int32 TileTimesRes = 1;
	int32 TileTimesFill = 0;

	static FOverlapTileInfo Rot90(FOverlapTileInfo InTile)
	{
		FOverlapTileInfo RetTile(InTile.TileSize);
		int32 size = InTile.TileSize;

		for (int r = 0; r < size; r++)
		{
			for (int c = 0; c < size; c++)
			{
				RetTile.Data[r][c] = InTile.Data[c][size - r -1];
			}
		}
		return RetTile;
	}
	static FOverlapTileInfo Rot180(FOverlapTileInfo InTile)
	{
		FOverlapTileInfo RetTile(InTile.TileSize);
		int32 size = InTile.TileSize;

		for (int r = 0; r < size; r++)
		{
			for (int c = 0; c < size; c++)
			{
				RetTile.Data[r][c] = InTile.Data[size - c - 1][size - r - 1];
			}
		}
		return RetTile;
	}
	static FOverlapTileInfo ReflectX(FOverlapTileInfo InTile)
	{
		FOverlapTileInfo RetTile(InTile.TileSize);
		int32 size = InTile.TileSize;

		for (int r = 0; r < size; r++)
		{
			for (int c = 0; c < size; c++)
			{
				RetTile.Data[r][c] = InTile.Data[r][size - c - 1];
			}
		}
		return RetTile;
	}
	static FOverlapTileInfo ReflectY(FOverlapTileInfo InTile)
	{
		FOverlapTileInfo RetTile(InTile.TileSize);
		int32 size = InTile.TileSize;

		for (int r = 0; r < size; r++)
		{
			for (int c = 0; c < size; c++)
			{
				RetTile.Data[r][c] = InTile.Data[size - r - 1][c];
			}
		}
		return RetTile;
	}
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONRUNTIME_API FOverlapNeighborInfo
{
	GENERATED_BODY()
	FOverlapNeighborInfo() {};
	FOverlapNeighborInfo(int32 L, int32 R)
	{
		Left = L;
		Right = R;
	}
	UPROPERTY(EditAnywhere)
	int32 Left;
	UPROPERTY(EditAnywhere)
	int32 Right;
	int num = 0;
};