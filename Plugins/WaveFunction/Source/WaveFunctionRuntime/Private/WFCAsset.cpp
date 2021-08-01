#include "WFCAsset.h"

UWFCAsset::UWFCAsset()
{
	BrushIndexSelected = -1;
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

void UWFCAsset::InitSetting()
{
	for (int r = 0; r < OutputRows; r++)
	{
		TArray<int32> TmpArray;
		for (int c = 0; c < OutputColumns; c++)
		{
			TmpArray.Add(-1);
		}
		OutputIndexList.Add(TmpArray);
	}
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);
	ReFillBrushes();
}

FSlateBrush* UWFCAsset::GetBrushInputByIndex(int32 Index)
{
	if (BrushesInput.IsValidIndex(Index))
	{
		return &BrushesInput[Index];
	}
	return nullptr;
}

FSlateBrush* UWFCAsset::GetBrushOutputByRowAndCloumns(int32 r, int32 c)
{
	if (OutputIndexList.IsValidIndex(r) && OutputIndexList[r].IsValidIndex(c))
	{
		FSlateBrush* ResultBrush = nullptr;
		if (BrushesOutput.IsValidIndex(OutputIndexList[r][c]))
		{
			ResultBrush = &BrushesOutput[OutputIndexList[r][c]];
		}
		if (!ResultBrush)
		{
			ResultBrush = &OutputInitBrush;
		}
		return ResultBrush;
	}
	return nullptr;
}

void UWFCAsset::SetBrushOutputByRowAndCloumns(int32 r, int32 c)
{
	if (OutputIndexList.IsValidIndex(r) && OutputIndexList[r].IsValidIndex(c))
	{
		OutputIndexList[r][c] = BrushIndexSelected;
	}
	PropertyChange.Broadcast();
}

void UWFCAsset::SaveCurrentOutPutAsTemplate()
{
	TemplateIndexList.Empty();
	int32 minr = OutputRows;
	int32 maxr = 0;
	int32 minc = OutputColumns;
	int32 maxc = 0;
	for (int r=0; r<OutputRows; r++)
	{
		for (int c=0; c<OutputColumns; c++)
		{
			if (OutputIndexList[r][c] != -1)
			{
				if (r < minr)
				{
					minr = r;
				}
				if (r > maxr)
				{
					maxr = r;
				}
				if (c < minc)
				{
					minc = c;
				}
				if (c > maxc)
				{
					maxc = c;
				}
			}
		}
	}
	for (int r = minr; r<=maxr; r++)
	{
		TArray<int32> TempArray;
		for (int c= minc; c<=maxc; c++)
		{
			TempArray.Add(OutputIndexList[r][c]);
		}
		TemplateIndexList.Add(TempArray);
	}
	OutputIndexList = TemplateIndexList;
	OutputRows = maxr - minr + 1;
	OutputColumns = maxc - minc + 1;
	PropertyChange.Broadcast();

}

void UWFCAsset::ReFillBrushes()
{
	BrushesInput.Empty();
	BrushesOutput.Empty();

	for (auto Texture : InputRes)
	{
		FSlateBrush TempBrushInput;
		TempBrushInput.SetResourceObject(Texture);
		//Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		TempBrushInput.ImageSize = FVector2D(64, 64);
		BrushesInput.Add(TempBrushInput);

		FSlateBrush TempBrushOutput;
		TempBrushOutput.SetResourceObject(Texture);
		TempBrushOutput.ImageSize = FVector2D(BrushSize, BrushSize);
		BrushesOutput.Add(TempBrushOutput);
	}
	if (BrushesInput.Num() > 0 && BrushIndexSelected == -1)
	{
		BrushIndexSelected = 0;
	}
}


void UWFCAsset::BrushIndexSelectedChange(int32 NewIndex)
{
	BrushIndexSelected = NewIndex;
}

int32 UWFCAsset::GetBrushIndexSelected()
{
	return BrushIndexSelected;
}

void UWFCAsset::BrushSizeChange(FString ChangeType)
{
	if (ChangeType == "Add")
	{
		BrushSize += 1;
	}
	else if(ChangeType == "Sub" && BrushSize > 0)
	{
		BrushSize -= 1;
	}
	ReSizeBrushesOutput();
	//PropertyChange.Broadcast();
}


void UWFCAsset::ReSizeBrushesOutput()
{
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);

	for (auto& Brush : BrushesOutput)
	{
		Brush.ImageSize = FVector2D(BrushSize, BrushSize);
	}
}

void UWFCAsset::ReFillOutputIndexList()
{
	OutputIndexList.Empty();

	for (int r = 0; r < OutputRows; r++)
	{
		TArray<int32> TmpArray;
		for (int c = 0; c < OutputColumns; c++)
		{
			TmpArray.Add(-1);
		}
		OutputIndexList.Add(TmpArray);
	}
}

void UWFCAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
		//&& PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, InputRes))
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, InputRes))
		{
			ReFillBrushes();
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, BrushSize))
		{
			ReSizeBrushesOutput();
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, OutputRows)
			|| PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, OutputColumns))
		{
			ReFillOutputIndexList();
		}

		PropertyChange.Broadcast();
	}
}
