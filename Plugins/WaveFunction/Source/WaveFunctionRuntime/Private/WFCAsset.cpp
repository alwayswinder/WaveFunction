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
	InitSetting(); 

	Symmetrys.Empty();
	for (int i = 0; i < BrushesInput.Num(); i++)
	{
		Symmetrys.Add(ESymmetry::I);
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
		if (IsPaint)
		{
			OutputIndexList[r][c] = BrushIndexSelected;
		}
		else
		{
			OutputIndexList[r][c] = -1;
		}
	}

	PropertyChangeOutput.Broadcast();
}

void UWFCAsset::ClearBrushOutput()
{
	ReFillOutputIndexList();
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
	PropertyChangeOutput.Broadcast();

}

int32 UWFCAsset::GetTilesNum()
{
	return BrushesInput.Num();
}

int32 UWFCAsset::GetSymmetrysNum()
{
	return Symmetrys.Num();
}


void UWFCAsset::ReSetIsPaint()
{
	IsPaint = !IsPaint;
}

bool UWFCAsset::GetIsPaint()
{
	return IsPaint;
}

void UWFCAsset::ReFillBrushes()
{
	BrushesInput.Empty();
	BrushesOutput.Empty();

	for (auto Texture : InputRes)
	{
		if (Texture)
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
	}
	if (BrushesInput.Num() > 0 && BrushIndexSelected == -1)
	{
		BrushIndexSelected = 0;
	}
	PropertyChangeInput.Broadcast();
}


void UWFCAsset::ReFillSymmetrys()
{
	if (Symmetrys.Num() == 0)
	{
		for (int i=0; i<BrushesInput.Num(); i++)
		{
			Symmetrys.Add(ESymmetry::I);
		}
	}
	else
	{
		int sub = BrushesInput.Num() - Symmetrys.Num();
		if (sub > 0)
		{
			for (int i = 0; i < sub; i++)
			{
				Symmetrys.RemoveAt(Symmetrys.Num()-1);
			}
		}
		else
		{
			for (int i = 0; i < sub; i++)
			{
				Symmetrys.Add(ESymmetry::I);
			}
		}
	}
}

void UWFCAsset::ReFillNeighbors()
{
	int32 key = 0;
	for (int i=0; i<GetTilesNum(); i++)
	{
		for (int j=0; j<GetTilesNum(); j++)
		{
			Neighbors.Add(key, FNeighborInfo(i, j));
			key++;
		}
	}
}

void UWFCAsset::RemoveNeighborByKey(int32 Key)
{
	Neighbors.Remove(Key);
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
}


void UWFCAsset::ReSizeBrushesOutput()
{
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);

	for (auto& Brush : BrushesOutput)
	{
		Brush.ImageSize = FVector2D(BrushSize, BrushSize);
	}
	PropertyChangeOutput.Broadcast();
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
	PropertyChangeOutput.Broadcast();
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
	}
}
