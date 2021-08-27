#include "WFCAsset.h"

UWFCAsset::UWFCAsset()
{
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
		TArray<FTilesInfo> TmpArray;
		for (int c = 0; c < OutputColumns; c++)
		{
			TmpArray.Add(FTilesInfo());
		}
		OutputTileInfoList.Add(TmpArray);
	}
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);
	ReFillBrushes();	
}

FSlateBrush* UWFCAsset::GetBrushInputByTileIndex(int32 Index)
{
	if (InputTileInfoList.IsValidIndex(Index))
	{
		if (BrushesInput.IsValidIndex(InputTileInfoList[Index].index))
		{
			return &BrushesInput[InputTileInfoList[Index].index];
		}
	}
	return nullptr;
}

FSlateBrush* UWFCAsset::GetBrushOutputByRowAndCloumns(int32 r, int32 c)
{
	if (OutputTileInfoList.IsValidIndex(r) && OutputTileInfoList[r].IsValidIndex(c))
	{
		FSlateBrush* ResultBrush = nullptr;
		int32 index = OutputTileInfoList[r][c].index;
		if (BrushesOutput.IsValidIndex(index))
		{
			ResultBrush = &BrushesOutput[index];
		}
		if (!ResultBrush)
		{
			ResultBrush = &OutputInitBrush;
		}
		return ResultBrush;
	}
	return nullptr;
}

ETileRot UWFCAsset::GetOutputTileRotByRowAndCloumns(int32 r, int32 c)
{
	if (OutputTileInfoList.IsValidIndex(r) && OutputTileInfoList[r].IsValidIndex(c))
	{
		return OutputTileInfoList[r][c].Rot;
	}
	return ETileRot::None;
}

void UWFCAsset::SetBrushOutputByRowAndCloumns(int32 r, int32 c)
{
	if (OutputTileInfoList.IsValidIndex(r) && OutputTileInfoList[r].IsValidIndex(c))
	{
		if (IsPaint)
		{
			OutputTileInfoList[r][c].index = InputTileInfoList[InputTileIndexSelected].index;
			OutputTileInfoList[r][c].Rot = InputTileInfoList[InputTileIndexSelected].Rot;
			OnOutputAnalysis(r, c);
		}
		else
		{
			OutputTileInfoList[r][c].index = -1;
			OutputTileInfoList[r][c].Rot = ETileRot::None;
		}
	}

	PropertyChangeOutput.Broadcast();
}

void UWFCAsset::ClearBrushOutput()
{
	ReFillOutputIndexList();
}

void UWFCAsset::OnOutputAnalysis(int r, int c)
{
	OutputTilesMaybe[r][c].Empty();
	FTilesInfo LastTile = InputTileInfoList[InputTileIndexSelected];
	//up
	if (OutputTilesMaybe.IsValidIndex(r-1) && OutputTilesMaybe[r-1].IsValidIndex(c)
		&& OutputTilesMaybe[r-1][c].Num() > 0)
	{ 
		LastTile.Rot = TileRotAdd(LastTile.Rot);
		TArray<int32> UpNeighbors;
		for (auto neighbor : Neighbors)
		{
			if (neighbor.Value.Left.index == LastTile.index
				&& neighbor.Value.Left.Rot == LastTile.Rot)
			{
				FTilesInfo Tmp = neighbor.Value.Right;
				Tmp.Rot = TileRotAdd(Tmp.Rot); 
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r - 1][c].Num(); i++)
				{
					if (Tmp.index == InputTileInfoList[OutputTilesMaybe[r - 1][c][i]].index
						&& Tmp.Rot == InputTileInfoList[OutputTilesMaybe[r - 1][c][i]].Rot)
					{
						IsFind = true;
					}
				}
				if (IsFind)
				{
					UpNeighbors.Add(Tmp.index);
				}
			}
		}
		OutputTilesMaybe[r - 1][c] = UpNeighbors;
	}
}

void UWFCAsset::OnOutputGenerate()
{

	PropertyChangeOutput.Broadcast();
}
/*
void UWFCAsset::SaveCurrentOutPutAsTemplate()
{
	TemplateIndexList.Empty();
	int32 minr = OutputRows;
	int32 maxr = 0;
	int32 minc = OutputColumns;
	int32 maxc = 0;
	for (int r = 0; r < OutputRows; r++)
	{
		for (int c = 0; c < OutputColumns; c++)
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
	for (int r = minr; r <= maxr; r++)
	{
		TArray<int32> TempArray;
		for (int c = minc; c <= maxc; c++)
		{
			TempArray.Add(OutputIndexList[r][c]);
		}
		TemplateIndexList.Add(TempArray);
	}
	OutputIndexList = TemplateIndexList;
	OutputRows = maxr - minr + 1;
	OutputColumns = maxc - minc + 1;
	PropertyChangeOutput.Broadcast();

}*/

int32 UWFCAsset::GetTilesNum()
{
	return BrushesInput.Num();
}

int32 UWFCAsset::GetSymmetrysNum()
{
	return Symmetrys.Num();
}


int32 UWFCAsset::GetOutputResultNumByRC(int32 r, int32 c)
{
	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c))
	{
		return OutputTilesMaybe[r][c].Num();
	}
	return 0;
}

ETileRot UWFCAsset::TileRotAdd(ETileRot inRot)
{
	ETileRot retRot;
	if (inRot == ETileRot::None)
	{
		retRot = ETileRot::nine;
	}
	if (inRot == ETileRot::nine)
	{
		retRot = ETileRot::OneEight;
	}
	if (inRot == ETileRot::OneEight)
	{
		retRot = ETileRot::TowSeven;
	}
	if (inRot == ETileRot::TowSeven)
	{
		retRot = ETileRot::None;
	}
	return retRot;
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
	ReFillInputBrushesWithRot();
	ReFillOutputIndexList();
	PropertyChangeInput.Broadcast();
}


void UWFCAsset::ReFillInputBrushesWithRot()
{
	InputTileInfoList.Empty();

	for (int i=0; i<BrushesInput.Num(); i++)
	{
		InputTileInfoList.Add(FTilesInfo(i, ETileRot::None));

		ESymmetry Sym = ESymmetry::X;
		if (Symmetrys.IsValidIndex(i))
		{
			Sym = Symmetrys[i];
		}
		switch (Sym)
		{
		case ESymmetry::I:
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::nine));
			break;
		case ESymmetry::X:
			break;
		case ESymmetry::T:
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::nine));
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::OneEight));
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::TowSeven));
			//InputTileInfoList.Add(FTilesInfo(i, ETileRot::UD));
			break;
		case ESymmetry::L:
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::nine));
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::OneEight));
			InputTileInfoList.Add(FTilesInfo(i, ETileRot::TowSeven));
			//InputTileInfoList.Add(FTilesInfo(i, ETileRot::LR));
			//InputTileInfoList.Add(FTilesInfo(i, ETileRot::UD));
			break;
		default:
			break;
		}
	}
	if (InputTileInfoList.Num() > 0)
	{
		InputTileIndexSelected = 0;
	}
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

	PropertyChangeOutput.Broadcast();
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

void UWFCAsset::InputileIndexSelectedChange(int32 NewIndex)
{
	InputTileIndexSelected = NewIndex;
}

int32 UWFCAsset::GetInputTileIndexSelected()
{
	return InputTileIndexSelected;
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


FSlateBrush* UWFCAsset::GetBrushInputByIndex(int32 Index)
{
	if (BrushesInput.IsValidIndex(Index))
	{
		return &BrushesInput[Index];
	}

	return nullptr;
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
	OutputTileInfoList.Empty();
	OutputTilesMaybe.Empty();

	for (int r = 0; r < OutputRows; r++)
	{
		TArray<FTilesInfo> TmpArray;
		TArray<TArray<int32>> Tmp32Array;
		for (int c = 0; c < OutputColumns; c++)
		{
			TmpArray.Add(FTilesInfo());
			TArray<int32> Tmp;
			for (int i=0; i<InputTileInfoList.Num(); i++)
			{
				Tmp.Add(i);
			}
			Tmp32Array.Add(Tmp);
		}
		OutputTileInfoList.Add(TmpArray);
		OutputTilesMaybe.Add(Tmp32Array);
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
