#include "WFCAsset.h"

UWFCAsset::UWFCAsset()
{
	for (int r=0; r<OutputRows;r++)
	{
		TArray<int32> TmpArray;
		for (int c=0; c<OutputColumns;c++)
		{
			TmpArray.Add(-1);
		}
		OutputIndexList.Add(TmpArray);
	}

	FSoftObjectPath TextureAsset(TEXT("Texture2D'/WaveFunction/Texture/Tile/medievalTile_58.medievalTile_58'"));
	UObject* TextureObject = TextureAsset.ResolveObject();
	if (!TextureObject)
	{
		TextureObject = TextureAsset.TryLoad();
	}
	if (TextureObject)
	{
		OutputInitTexture = Cast<UTexture2D>(TextureObject);
	}
	OutputInitBrush.SetResourceObject(OutputInitTexture);
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);
}

void UWFCAsset::InitResBase(TArray<FAssetData>& InResBase)
{
	for (auto AsserData:InResBase)
	{
		UTexture2D* Temp = Cast<UTexture2D>(AsserData.GetAsset());
		if (Temp)
		{
			InputRes.Add(Temp);
		}
	}
}

FSlateBrush* UWFCAsset::GetBrushByIndex(int32 Index)
{
	if (Brushes.IsValidIndex(Index))
	{
		return &Brushes[Index];
	}
	return nullptr;
}

FSlateBrush* UWFCAsset::GetBrushByRowAndCloumns(int32 r, int32 c)
{
	if (OutputIndexList.IsValidIndex(r) && OutputIndexList[r].IsValidIndex(c))
	{
		FSlateBrush* ResultBrush = GetBrushByIndex(OutputIndexList[r][c]);
		if (!ResultBrush)
		{
			ResultBrush = &OutputInitBrush;
		}
		return ResultBrush;
	}
	return nullptr;
}

void UWFCAsset::ReFillBrushes()
{
	Brushes.Empty();
	for (auto Texture : InputRes)
	{
		FSlateBrush TempBrush;
		TempBrush.SetResourceObject(Texture);
		//Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		TempBrush.ImageSize = FVector2D(BrushSize, BrushSize);
		Brushes.Add(TempBrush);
	}
}

void UWFCAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
		//&& PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, InputRes))
	{
		PropertyChange.Broadcast();
	}
}
