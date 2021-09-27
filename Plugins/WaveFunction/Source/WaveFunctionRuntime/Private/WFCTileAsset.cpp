#include "WFCTileAsset.h"
#include "FileHelpers.h"


UWFCTileAsset::UWFCTileAsset()
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

void UWFCTileAsset::InitResBase(TArray<FAssetData>& InResBase)
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

void UWFCTileAsset::InitSetting()
{
	int k = 0;
	OutputTileInfoList.Empty();
	for (int r = 0; r < OutputRows; r++)
	{
		TArray<FTilesInfo> TmpArray;
		for (int c = 0; c < OutputColumns; c++)
		{
			if (ResultListSave.IsValidIndex(k))
			{
				TmpArray.Add(ResultListSave[k]);
			}
			else
			{
				TmpArray.Add(FTilesInfo());
			}
			k++;
		}
		OutputTileInfoList.Add(TmpArray);
	}
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);
	ReFillBrushes();	
}

FSlateBrush* UWFCTileAsset::GetBrushInputByTileIndex(int32 Index)
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

FSlateBrush* UWFCTileAsset::GetBrushOutputByRowAndCloumns(int32 r, int32 c)
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

ETileRot UWFCTileAsset::GetOutputTileRotByRowAndCloumns(int32 r, int32 c)
{
	if (OutputTileInfoList.IsValidIndex(r) && OutputTileInfoList[r].IsValidIndex(c))
	{
		return OutputTileInfoList[r][c].Rot;
	}
	return ETileRot::None;
}

void UWFCTileAsset::SetBrushOutputByRowAndCloumns(int32 r, int32 c)
{
	if (OutputTileInfoList.IsValidIndex(r) && OutputTileInfoList[r].IsValidIndex(c))
	{
		if (IsPaint)
		{
			OutputTileInfoList[r][c].index = InputTileInfoList[InputTileIndexSelected].index;
			OutputTileInfoList[r][c].Rot = InputTileInfoList[InputTileIndexSelected].Rot;
			OutputTimesApear[InputTileIndexSelected]++;
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

void UWFCTileAsset::ClearBrushOutput()
{
	ReFillOutputIndexList();
	SumFrequency = 0;

	OutputTimesApear.Empty();
	OutputFresuqnceShould.Empty();
	for (int i=0; i<InputTileInfoList.Num(); i++)
	{
		OutputTimesApear.Add(0);
		FTilesInfo tmp = InputTileInfoList[i];
		switch (Symmetrys[tmp.index])
		{
		case ESymmetry::I:
			OutputFresuqnceShould.Add(Frequencys[tmp.index] / 2);
			break;
		case ESymmetry::L:
			OutputFresuqnceShould.Add(Frequencys[tmp.index] / 4);
			break;
		case ESymmetry::T:
			OutputFresuqnceShould.Add(Frequencys[tmp.index] / 4);
			break;
		case ESymmetry::X:
			OutputFresuqnceShould.Add(Frequencys[tmp.index]);
			break;
		default:
			break;
		}
		SumFrequency += OutputFresuqnceShould[i];
	}
}

void UWFCTileAsset::SaveTileInfoOutput()
{
	ResultListSave.Empty();
	for (int r=0; r<OutputTileInfoList.Num();r++)
	{
		for (int c = 0; c < OutputTileInfoList[r].Num(); c++)
		{
			ResultListSave.Add(OutputTileInfoList[r][c]);
		}
	}
	//this->MarkPackageDirty();
}

void UWFCTileAsset::OnOutputAnalysis(int r, int c)
{
	OutputTilesMaybe[r][c].Empty();
	FTilesInfo LastTile = OutputTileInfoList[r][c];
	//up down left right
	if (OutputTilesMaybe.IsValidIndex(r-1) && OutputTilesMaybe[r-1].IsValidIndex(c)
		&& OutputTilesMaybe[r-1][c].Num() > 0)
	{ 
		TArray<FTilesInfo> UpNeighbors;
		for (auto neighbor : NeighborsUD)
		{
			if (neighbor.Right.index == LastTile.index && neighbor.Right.Rot == LastTile.Rot)
			{
				FTilesInfo Tmp = neighbor.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r - 1][c].Num(); i++)
				{
					if (Tmp.index == OutputTilesMaybe[r - 1][c][i].index
						&& Tmp.Rot == OutputTilesMaybe[r - 1][c][i].Rot)
					{
						IsFind = true;
					}
				}
				if (IsFind)
				{
					UpNeighbors.Add(Tmp);
				}
			}
		}
		OutputTilesMaybe[r - 1][c] = UpNeighbors;
	}

	if (OutputTilesMaybe.IsValidIndex(r + 1) && OutputTilesMaybe[r + 1].IsValidIndex(c)
		&& OutputTilesMaybe[r + 1][c].Num() > 0)
	{
		TArray<FTilesInfo> DownNeighbors;
		for (auto neighbor : NeighborsUD)
		{
			if (neighbor.Left.index == LastTile.index && neighbor.Left.Rot == LastTile.Rot)
			{
				FTilesInfo Tmp = neighbor.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r + 1][c].Num(); i++)
				{
					if (Tmp.index == OutputTilesMaybe[r + 1][c][i].index
						&& Tmp.Rot == OutputTilesMaybe[r + 1][c][i].Rot)
					{
						IsFind = true;
					}
				}
				if (IsFind)
				{
					DownNeighbors.Add(Tmp);
				}
			}
		}
		OutputTilesMaybe[r + 1][c] = DownNeighbors;
	}

	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c - 1)
		&& OutputTilesMaybe[r][c - 1].Num() > 0)
	{
		TArray<FTilesInfo> LeftNeighbors;
		for (auto neighbor : NeighborsLR)
		{
			if (neighbor.Right.index == LastTile.index && neighbor.Right.Rot == LastTile.Rot)
			{
				FTilesInfo Tmp = neighbor.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r][c - 1].Num(); i++)
				{
					if (Tmp.index == OutputTilesMaybe[r][c - 1][i].index
						&& Tmp.Rot == OutputTilesMaybe[r][c - 1][i].Rot)
					{
						IsFind = true;
					}
				}
				if (IsFind)
				{
					LeftNeighbors.Add(Tmp);
				}
			}
		}
		OutputTilesMaybe[r][c - 1] = LeftNeighbors;
	}

	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c + 1)
		&& OutputTilesMaybe[r][c + 1].Num() > 0)
	{
		TArray<FTilesInfo> RightNeighbors;
		for (auto neighbor : NeighborsLR)
		{
			if (neighbor.Left.index == LastTile.index && neighbor.Left.Rot == LastTile.Rot)
			{
				FTilesInfo Tmp = neighbor.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r][c + 1].Num(); i++)
				{
					if (Tmp.index == OutputTilesMaybe[r][c + 1][i].index
						&& Tmp.Rot == OutputTilesMaybe[r][c + 1][i].Rot)
					{
						IsFind = true;
					}
				}
				if (IsFind)
				{
					RightNeighbors.Add(Tmp);
				}
			}
		}
		OutputTilesMaybe[r][c + 1] = RightNeighbors;
	}
	int32 MaxMaybe = InputTileInfoList.Num();

	IsAllFilled = true;

	for (int R=0; R<OutputTilesMaybe.Num(); R++)
	{
		for (int32 C = 0; C < OutputTilesMaybe[R].Num(); C++)
		{
			if (OutputTilesMaybe[R][C].Num() > 0 && OutputTilesMaybe[R][C].Num() < MaxMaybe)
			{
				MaxMaybe = OutputTilesMaybe[R][C].Num();
				RowNext = R;
				ColumnNext = C;
			}

			if (OutputTilesMaybe[R][C].Num() > 0)
			{
				IsAllFilled = false;
			}
		}
	}
}

void UWFCTileAsset::OnOutputGenerate()
{
	float MinFrequency = 1.f;
	int32 indexSelect = 0;
	int32 indexSet = 0;
	int index = 0;

	for (int i = 0; i < OutputTilesMaybe[RowNext][ColumnNext].Num(); i++)
	{
		for (index = 0; index < InputTileInfoList.Num(); index++)
		{
			if (OutputTilesMaybe[RowNext][ColumnNext][i].index == InputTileInfoList[index].index
				&& OutputTilesMaybe[RowNext][ColumnNext][i].Rot == InputTileInfoList[index].Rot)
			{
				break;
			}
		}
		float FrequenceNow = 1.0f * OutputTimesApear[index] / (OutputRows * OutputColumns);
		float FrequenceShould = OutputFresuqnceShould[index] / SumFrequency;
		float FreTmp = FrequenceNow / FrequenceShould;
		if (FreTmp < MinFrequency)
		{
			MinFrequency = FrequenceNow / FrequenceShould;
			indexSelect = i;
			indexSet = index;
		}
	}
	if (OutputTilesMaybe.IsValidIndex(RowNext) && OutputTilesMaybe[RowNext].IsValidIndex(ColumnNext)
		&& OutputTilesMaybe[RowNext][ColumnNext].IsValidIndex(indexSelect))
	{
		OutputTileInfoList[RowNext][ColumnNext] = OutputTilesMaybe[RowNext][ColumnNext][indexSelect];

		OutputTimesApear[indexSet]++;

		OnOutputAnalysis(RowNext, ColumnNext);

		PropertyChangeOutput.Broadcast();
	}

	for (int i = 0; i < OutputTimesApear.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("index=%d, times=%d"), i, OutputTimesApear[i]);
	}
}

void UWFCTileAsset::OnOutputFill()
{
	while (!IsAllFilled)
	{
		OnOutputGenerate();
	}
}

void UWFCTileAsset::SaveRoadInfo()
{
	FString AssetPath = this->GetPathName();
	UE_LOG(LogTemp, Warning, TEXT("path = %s"), &AssetPath);

	UTexture2D* RoadInfoTexture;
	RoadInfoTexture = NewObject<UTexture2D>(this);

	RoadInfoTexture->PlatformData = new FTexturePlatformData();
	RoadInfoTexture->PlatformData->SizeX = 1024;
	RoadInfoTexture->PlatformData->SizeY = 1024;
	RoadInfoTexture->Filter = TextureFilter::TF_Nearest;
	RoadInfoTexture->MipGenSettings = TMGS_NoMipmaps;


	int32 TextureX = RoadInfoTexture->PlatformData->SizeX;
	int32 TextureY = RoadInfoTexture->PlatformData->SizeY;
	FLinearColor InColor = FLinearColor(1.0, 0.0, 0.0, 1.0);
	RoadInfoTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	TextureCompressionSettings OldCompressionSettings = RoadInfoTexture->CompressionSettings;
	TextureMipGenSettings OldMipGenSettings = RoadInfoTexture->MipGenSettings;
	bool OldSRGB = RoadInfoTexture->SRGB;

	RoadInfoTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	RoadInfoTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	RoadInfoTexture->SRGB = false;
	RoadInfoTexture->UpdateResource();

	uint8* Pixels = new uint8[TextureX * TextureY * 4];
	for (int32 y = 0; y < TextureY; y++)
	{
		for (int32 x = 0; x < TextureX; x++)
		{
			int32 curPixelIndex = ((y * TextureX) + x);
			//这里可以设置4个通道的值
			//这里需要考虑像素格式，之前设置了PF_B8G8R8A8，那么这里通道的顺序就是BGRA
			Pixels[4 * curPixelIndex] = InColor.R;
			Pixels[4 * curPixelIndex + 1] = InColor.G;
			Pixels[4 * curPixelIndex + 2] = InColor.B;
			Pixels[4 * curPixelIndex + 3] = InColor.A;
		}
	}
	//创建第一个MipMap
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	RoadInfoTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = TextureX;
	Mip->SizeY = TextureY;

	//锁定Texture让它可以被修改
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(TextureX * TextureY * 4);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * TextureX * TextureY * 4);
	Mip->BulkData.Unlock();

	//通过以上步骤，我们完成数据的临时写入
	//执行完以下这两个步骤，编辑器中的asset会显示可以保存的状态（如果是指定Asset来获取UTexture2D的指针的情况下）
	RoadInfoTexture->Source.Init(TextureX, TextureY, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);

	RoadInfoTexture->CompressionSettings = OldCompressionSettings;
	RoadInfoTexture->MipGenSettings = OldMipGenSettings;
	RoadInfoTexture->SRGB = OldSRGB;
	RoadInfoTexture->UpdateResource();

	RoadInfoTexture->GetPackage()->MarkPackageDirty();
	TArray<UPackage*> Packages;
	Packages.Add(RoadInfoTexture->GetPackage());
	UEditorLoadingAndSavingUtils::SavePackages(Packages, false);

	delete[] Pixels;
}

int32 UWFCTileAsset::GetTilesNum()
{
	return BrushesInput.Num();
}

int32 UWFCTileAsset::GetSymmetrysNum()
{
	return Symmetrys.Num();
}


int32 UWFCTileAsset::GetOutputResultNumByRC(int32 r, int32 c)
{
	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c))
	{
		return OutputTilesMaybe[r][c].Num();
	}
	return 0;
}

void UWFCTileAsset::ReSetIsPaint()
{
	IsPaint = !IsPaint;
}

bool UWFCTileAsset::GetIsPaint()
{
	return IsPaint;
}

void UWFCTileAsset::ReFillBrushes()
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
	//ReFillOutputIndexList();
	PropertyChangeInput.Broadcast();
	PropertyChangeOutput.Broadcast();
}


void UWFCTileAsset::ReFillInputBrushesWithRot()
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

void UWFCTileAsset::ReFillSymmetrysAndFrequencys()
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
	Frequencys.Empty();
	for (int i=0; i<BrushesInput.Num(); i++)
	{
		Frequencys.Add(1.0f);
	}

	PropertyChangeOutput.Broadcast();
}

void UWFCTileAsset::ReFillNeighbors()
{
	int32 key = 0;
	for (int i=0; i< InputTileInfoList.Num(); i++)
	{
		for (int j=i; j< InputTileInfoList.Num(); j++)
		{
			FNeighborInfo Neighbor;
			Neighbor.Left = InputTileInfoList[i];
			Neighbor.Right = InputTileInfoList[j];
			Neighbors.Add(key, Neighbor);
			key++;
		}
	}
}

void UWFCTileAsset::RemoveNeighborByKey(int32 Key)
{
	Neighbors.Remove(Key);
}

void UWFCTileAsset::ReFillNeighborLRAndUD()
{
	NeighborsLR.Empty();
	NeighborsUD.Empty();

	for (auto Neighbor : Neighbors)
	{
		NeighborsLR.Add(Neighbor.Value);
		FNeighborInfo Tmp;
		Tmp.Left = TileRot180(Neighbor.Value.Right);
		Tmp.Right = TileRot180(Neighbor.Value.Left);
		NeighborsLR.Add(Tmp);
	}
	for (int32 i = 0; i < NeighborsLR.Num(); i++)
	{
		FNeighborInfo Tmp;
		Tmp.Left = TileRot90(NeighborsLR[i].Left);
		Tmp.Right = TileRot90(NeighborsLR[i].Right);
		NeighborsUD.Add(Tmp);
	}
}

void UWFCTileAsset::InputileIndexSelectedChange(int32 NewIndex)
{
	InputTileIndexSelected = NewIndex;
}

int32 UWFCTileAsset::GetInputTileIndexSelected()
{
	return InputTileIndexSelected;
}

void UWFCTileAsset::BrushSizeChange(FString ChangeType)
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

FSlateBrush* UWFCTileAsset::GetBrushInputByIndex(int32 Index)
{
	if (BrushesInput.IsValidIndex(Index))
	{
		return &BrushesInput[Index];
	}

	return nullptr;
}

void UWFCTileAsset::ReSizeBrushesOutput()
{
	OutputInitBrush.ImageSize = FVector2D(BrushSize, BrushSize);

	for (auto& Brush : BrushesOutput)
	{
		Brush.ImageSize = FVector2D(BrushSize, BrushSize);
	}
	PropertyChangeOutput.Broadcast();
}

void UWFCTileAsset::ReFillOutputIndexList()
{
	OutputTileInfoList.Empty();
	OutputTilesMaybe.Empty();

	RowNext = -1;
	ColumnNext = -1;

	for (int r = 0; r < OutputRows; r++)
	{
		TArray<FTilesInfo> TmpArray;
		TArray<TArray<FTilesInfo>> TmpMaybeArray;
		for (int c = 0; c < OutputColumns; c++)
		{
			TmpArray.Add(FTilesInfo());
			TArray<FTilesInfo> Tmp;
			for (int i=0; i<InputTileInfoList.Num(); i++)
			{
				Tmp.Add(InputTileInfoList[i]);
			}
			TmpMaybeArray.Add(Tmp);
		}
		OutputTileInfoList.Add(TmpArray);
		OutputTilesMaybe.Add(TmpMaybeArray);
	}

	PropertyChangeOutput.Broadcast();
}

FTilesInfo UWFCTileAsset::TileRot180(FTilesInfo InTile)
{
	FTilesInfo Tmp = InTile;
	switch (Symmetrys[InTile.index])
	{
	case ESymmetry::I:
		break;
	case ESymmetry::L:
		switch (InTile.Rot)
		{
		case ETileRot::None:
			Tmp.Rot = ETileRot::OneEight;
			break;
		case ETileRot::nine:
			Tmp.Rot = ETileRot::TowSeven;
			break;
		case ETileRot::OneEight:
			Tmp.Rot = ETileRot::None;
			break;
		case ETileRot::TowSeven:
			Tmp.Rot = ETileRot::nine;
			break;
		default:
			break;
		}
		break;
	case ESymmetry::T:
		switch (InTile.Rot)
		{
		case ETileRot::None:
			Tmp.Rot = ETileRot::OneEight;
			break;
		case ETileRot::nine:
			Tmp.Rot = ETileRot::TowSeven;
			break;
		case ETileRot::OneEight:
			Tmp.Rot = ETileRot::None;
			break;
		case ETileRot::TowSeven:
			Tmp.Rot = ETileRot::nine;
			break;
		default:
			break;
		}
		break;
	case ESymmetry::X:
		break;
	default:
		break;
	}
	return Tmp;
}

FTilesInfo UWFCTileAsset::TileRot90(FTilesInfo InTile)
{
	FTilesInfo Tmp = InTile;
	switch (Symmetrys[InTile.index])
	{
	case ESymmetry::I:
		switch (InTile.Rot)
		{
		case ETileRot::None:
			Tmp.Rot = ETileRot::nine;
			break;
		case ETileRot::nine:
			Tmp.Rot = ETileRot::None;
			break;
		default:
			break;
		}
		break;
	case ESymmetry::L:
		switch (InTile.Rot)
		{
		case ETileRot::None:
			Tmp.Rot = ETileRot::nine;
			break;
		case ETileRot::nine:
			Tmp.Rot = ETileRot::OneEight;
			break;
		case ETileRot::OneEight:
			Tmp.Rot = ETileRot::TowSeven;
			break;
		case ETileRot::TowSeven:
			Tmp.Rot = ETileRot::None;
			break;
		default:
			break;
		}
		break;
	case ESymmetry::T:
		switch (InTile.Rot)
		{
		case ETileRot::None:
			Tmp.Rot = ETileRot::nine;
			break;
		case ETileRot::nine:
			Tmp.Rot = ETileRot::OneEight;
			break;
		case ETileRot::OneEight:
			Tmp.Rot = ETileRot::TowSeven;
			break;
		case ETileRot::TowSeven:
			Tmp.Rot = ETileRot::None;
			break;
		default:
			break;
		}
		break;
	case ESymmetry::X:
		break;
	default:
		break;
	}
	return Tmp;
}

void UWFCTileAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
		//&& PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, InputRes))
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCTileAsset, InputRes))
		{
			ReFillBrushes();
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCTileAsset, BrushSize))
		{
			ReSizeBrushesOutput();
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCTileAsset, OutputRows)
			|| PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCTileAsset, OutputColumns))
		{
			ReFillOutputIndexList();
		}
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCTileAsset, Symmetrys))
		{
			PropertyChangeInput.Broadcast();
		}
	}
}
