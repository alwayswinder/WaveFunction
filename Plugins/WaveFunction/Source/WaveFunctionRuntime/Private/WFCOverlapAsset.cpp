#include "WFCOverlapAsset.h"
#include "AssetRegistry\AssetRegistryModule.h"

UWFCOverlapAsset::UWFCOverlapAsset()
{
}

void UWFCOverlapAsset::BrushSizeChange(FString ChangeType /* = "Add" */)
{
	if (ChangeType == "Add")
	{
		OutputScale *= 1.1;
	}
	else if (ChangeType == "Sub" && OutputScale > 0)
	{
		OutputScale *= 0.9;
	}
	PropertyChangeOutput.Broadcast();
}

void UWFCOverlapAsset::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UWFCOverlapAsset* This = CastChecked<UWFCOverlapAsset>(InThis);
	Collector.AddReferencedObject(This->OutputTexture);

	Super::AddReferencedObjects(InThis, Collector);
}

void UWFCOverlapAsset::Analyse()
{
	if (!InputImage)
	{
		return;
	}

	AllOverlapTiles.Empty();
	PixelInput.Empty();

	ReadFromTexture();

	PixelRC.Empty();

	int32 Row = InputImage->PlatformData->SizeX;
	int32 Column = InputImage->PlatformData->SizeY;
	for (int32 Y = 0; Y < Column; Y++)
	{
		TArray<FPixelInfo> TmpPixel;
		for (int32 X = 0; X < Row; X++)
		{
			TmpPixel.Add(PixelInput[Y * Row + X]);
		}
		PixelRC.Add(TmpPixel);
	}

	AllNeighborsLR.Empty();
	AllNeighborsUd.Empty();

	for (int c = 0; c<Column; c++)
	{
		for (int r = 0; r< Row; r++)
		{
			FOverlapTileInfo PixelHere = FindTileFormArray(r, c,Row, Column, PixelRC);

			CheckAndAddTileInAll(PixelHere);

			if (UseRotAndReflect)
			{
				FOverlapTileInfo PixelHereRot90 = FOverlapTileInfo::Rot90(PixelHere);
				CheckAndAddTileInAll(PixelHereRot90);

				FOverlapTileInfo PixelHereRot180 = FOverlapTileInfo::Rot180(PixelHere);
				CheckAndAddTileInAll(PixelHereRot180);


				FOverlapTileInfo PixelHereRot270 = FOverlapTileInfo::Rot90(PixelHereRot180);
				CheckAndAddTileInAll(PixelHereRot270);

				FOverlapTileInfo PixelHereReflectX = FOverlapTileInfo::ReflectX(PixelHere);
				CheckAndAddTileInAll(PixelHereReflectX);

				FOverlapTileInfo PixelHereReflectY = FOverlapTileInfo::ReflectX(PixelHere);
				CheckAndAddTileInAll(PixelHereReflectY);
			}
		}
	}
	for (int i=0; i< AllOverlapTiles.Num(); i++)
	{
		for (int j=0; j < AllOverlapTiles.Num(); j++)
		{
			AddNeighBorsLR(i, j);
			AddNeighBorsUD(i, j);
		}
	}

	OutputTileIndex.Empty();
	OutputTilesMaybe.Empty();

	TArray<int32> AllIndexMaybe;
	for (int i=0; i<AllOverlapTiles.Num(); i++)
	{
		AllIndexMaybe.Add(i);
	}
	for (int c=0; c<OutputColumn; c++)
	{
		TArray<int32> Tmp;
		TArray<TArray<int32>> TmpMaybe;
		for (int r=0; r<OutputRow; r++)
		{
			Tmp.Add(-1);
			TmpMaybe.Add(AllIndexMaybe);
		}
		OutputTileIndex.Add(Tmp);
		OutputTilesMaybe.Add(TmpMaybe);
	}

	int R = OutputRow / 2;
	int C = OutputColumn / 2;

	OutputTileIndex[C][R] = FMath::RandRange(0, AllOverlapTiles.Num() - 1);
	OutputTilesMaybe[C][R].Empty();

	OnAnalyseStep(R,C);
	FillOutput();
}

void UWFCOverlapAsset::ClearOutput()
{
	for (int32 y = 0; y < OutputColumn; y++)
	{
		for (int32 x = 0; x < OutputRow; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			PixelsOutput[curPixelIndex].R = 255;
			PixelsOutput[curPixelIndex].G = 255;
			PixelsOutput[curPixelIndex].B = 255;
			PixelsOutput[curPixelIndex].A = 255;
		}
	}
	WriteToTexture();
}

void UWFCOverlapAsset::FillOutput()
{
	int Steps = 0;
	while(Steps < StepOnce)
	{
		bool IsAllSet = true;
		int MinMaybeNum = AllOverlapTiles.Num() + 1;
		int NextR = 0;
		int NextC = 0;
		for (int32 C = 0; C < OutputColumn; C++)
		{
			for (int32 R = 0; R < OutputRow; R++)
			{
				if (OutputTilesMaybe[C][R].Num() > 0 && OutputTilesMaybe[C][R].Num() < MinMaybeNum)
				{
					MinMaybeNum = OutputTilesMaybe[C][R].Num();
					NextR = R;
					NextC = C;
				}
				if (OutputTilesMaybe[C][R].Num() > 0)
				{
					IsAllSet = false;
				}
			}
		}
		if (IsAllSet)
		{
			break;
		}
		bool IsRestart = false;
		for (int c = 0; c < OutputTilesMaybe.Num(); c++)
		{
			for (int r = 0; r < OutputTilesMaybe[c].Num(); r++)
			{
				if (OutputTilesMaybe[c][r].Num() == 1 && c!=NextC && r!=NextR)
				{
					OutputTileIndex[c][r] = OutputTilesMaybe[c][r][0];
					AllOverlapTiles[OutputTileIndex[c][r]].TileTimesFill += 1;
					OutputTilesMaybe[c][r].Empty();
					OnAnalyseStep(r, c);
					IsRestart = true;
					break;
				}
			}
			if (IsRestart)
			{
				break;
			}
		}

		if (IsRestart)
		{
			continue;
		}

		int index = 0;
		//int32 index = FMath::RandRange(0, OutputTilesMaybe[NextR][NextC].Num() - 1);
		if (OutputTilesMaybe[NextC][NextR].Num() > 0)
		{
			float MinNum = 1.0f * OutputRow * OutputColumn + 1;
			for (int k=0; k< OutputTilesMaybe[NextC][NextR].Num(); k++)
			{
				int tmpindex = OutputTilesMaybe[NextC][NextR][k];

				float TimesNow = 1.0f * AllOverlapTiles[tmpindex].TileTimesFill;
				float TimesShould = 1.0f * AllOverlapTiles[tmpindex].TileTimesRes/ 
					(1.0f * InputImage->PlatformData->SizeX * InputImage->PlatformData->SizeY)*
					(1.0f * OutputRow * OutputColumn);

				float subNum = TimesNow / TimesShould;
				//UE_LOG(LogTemp, Warning, TEXT("sub=%f"), subNum);
				if (subNum < MinNum)
				{
					MinNum = subNum;
					index = k;
					//UE_LOG(LogTemp, Warning, TEXT("Min=%f"), MinNum);
				}
			}
		}
		if (OutputTilesMaybe[NextC][NextR].IsValidIndex(index))
		{
			OutputTileIndex[NextC][NextR] = OutputTilesMaybe[NextC][NextR][index];
			AllOverlapTiles[OutputTileIndex[NextC][NextR]].TileTimesFill += 1;
			OutputTilesMaybe[NextC][NextR].Empty();
		}
		OnAnalyseStep(NextR, NextC);
		Steps++;
	}

	for (int32 y = 0; y < OutputColumn; y++)
	{
		for (int32 x = 0; x < OutputRow; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			FOverlapTileInfo Tile(AnalyseSize);
			if (OutputTileIndex[y][x] != -1)
			{
				Tile = AllOverlapTiles[OutputTileIndex[y][x]];
			}
			else if (OutputTilesMaybe[y][x].Num()>0)
			{
				int R = 0, G = 0, B = 0, A = 0;
				for (int32 k =0; k< OutputTilesMaybe[y][x].Num(); k++)
				{
					R += AllOverlapTiles[OutputTilesMaybe[y][x][k]].Data[0][0].R;
					G += AllOverlapTiles[OutputTilesMaybe[y][x][k]].Data[0][0].G;
					B += AllOverlapTiles[OutputTilesMaybe[y][x][k]].Data[0][0].B;
					A += AllOverlapTiles[OutputTilesMaybe[y][x][k]].Data[0][0].A;
				}
				Tile.Data[0][0].R = R / OutputTilesMaybe[y][x].Num();
				Tile.Data[0][0].G = G / OutputTilesMaybe[y][x].Num();
				Tile.Data[0][0].B = B / OutputTilesMaybe[y][x].Num();
				Tile.Data[0][0].A = A / OutputTilesMaybe[y][x].Num();

			}
			else
			{
				Tile = AllOverlapTiles[FMath::RandRange(0, AllOverlapTiles.Num()-1)];
			}
			PixelsOutput[curPixelIndex].R = Tile.Data[0][0].R;
			PixelsOutput[curPixelIndex].G = Tile.Data[0][0].G;
			PixelsOutput[curPixelIndex].B = Tile.Data[0][0].B;
			PixelsOutput[curPixelIndex].A = Tile.Data[0][0].A;
		}
	}
	WriteToTexture();

	PropertyChangeOutput.Broadcast();
}

void UWFCOverlapAsset::InitCreated()
{
	PixelsOutput.Empty();
	for (int32 y = 0; y < OutputColumn; y++)
	{
		for (int32 x = 0; x < OutputRow; x++)
		{	
			int32 curPixelIndex = ((y * OutputRow) + x);

			PixelsOutput.Add(FPixelInfo(255, 255, 255, 255));
		}
	}
	OutputTexture = NewObject<UTexture2D>(this);
	//OutputTexture = CreateDefaultSubobject<UTexture2D>(TEXT("OutputTexture2d"));
	//OutputTexture->AddToRoot();
	OutputTexture->PlatformData = new FTexturePlatformData();
	OutputTexture->PlatformData->SizeX = OutputRow;
	OutputTexture->PlatformData->SizeY = OutputColumn;
	OutputTexture->Filter = TextureFilter::TF_Nearest;
	//设置像素格式
	OutputTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
	OutputTexture->NeverStream = true;
	OutputTexture->MipGenSettings = TMGS_NoMipmaps;

	OutputBrush.SetResourceObject(OutputTexture);
	OutputBrush.ImageSize = FVector2D(OutputRow, OutputColumn);

	WriteToTexture();
}

void UWFCOverlapAsset::WriteToTexture()
{
	//创建一个uint8的数组并取得指针
	//这里需要考虑之前设置的像素格式
	if (PixelsOutput.Num() <= 0)
	{
		return;
	}

	uint8* Pixels = new uint8[OutputRow * OutputColumn * 4];
	for (int32 y = 0; y < OutputColumn; y++)
	{
		for (int32 x = 0; x < OutputRow; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			//这里可以设置4个通道的值
			//这里需要考虑像素格式，之前设置了PF_B8G8R8A8，那么这里通道的顺序就是BGRA
			Pixels[4 * curPixelIndex] = PixelsOutput[curPixelIndex].B;
			Pixels[4 * curPixelIndex + 1] = PixelsOutput[curPixelIndex].G;
			Pixels[4 * curPixelIndex + 2] = PixelsOutput[curPixelIndex].R;
			Pixels[4 * curPixelIndex + 3] = PixelsOutput[curPixelIndex].A;
		}
	}
	//创建第一个MipMap
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	OutputTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = OutputRow;
	Mip->SizeY = OutputColumn;

	//锁定Texture让它可以被修改
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(OutputRow * OutputColumn * 4);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * OutputRow * OutputColumn * 4);
	Mip->BulkData.Unlock();

	//通过以上步骤，我们完成数据的临时写入
	//执行完以下这两个步骤，编辑器中的asset会显示可以保存的状态（如果是指定Asset来获取UTexture2D的指针的情况下）
	OutputTexture->Source.Init(OutputRow, OutputColumn, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);
	OutputTexture->UpdateResource();
	
	GetPackage()->MarkPackageDirty();
	//FAssetRegistryModule::AssetCreated(OutputTexture);
	//通过asset路径获取包中文件名
	//FString AssetPath = TEXT("/Game") + GetPackage()->GetPathName() + TEXT("/") + TEXT("OutputTexture2d");

	//FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
	//进行保存
	//bool bSaved = UPackage::SavePackage(GetPackage(), OutputTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	delete[] Pixels;
}

void UWFCOverlapAsset::ReadFromTexture()
{
	TextureCompressionSettings OldCompressionSettings = InputImage->CompressionSettings;
	TextureMipGenSettings OldMipGenSettings = InputImage->MipGenSettings;
	bool OldSRGB = InputImage->SRGB;

	InputImage->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	InputImage->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	InputImage->SRGB = false;
	InputImage->UpdateResource();

	const FColor* FormatedImageData = static_cast<const FColor*>(InputImage->PlatformData->Mips[0].BulkData.LockReadOnly());

	for (int32 Y = 0; Y < InputImage->PlatformData->SizeY; Y++)
	{
		for (int32 X = 0; X < InputImage->PlatformData->SizeX; X++)
		{
			FColor PixelColor = FormatedImageData[Y * InputImage->PlatformData->SizeX + X];
			//做若干操作
			PixelInput.Add(FPixelInfo(PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A));
		}
	}

	InputImage->PlatformData->Mips[0].BulkData.Unlock();

	InputImage->CompressionSettings = OldCompressionSettings;
	InputImage->MipGenSettings = OldMipGenSettings;
	InputImage->SRGB = OldSRGB;
	InputImage->UpdateResource();
}

void UWFCOverlapAsset::OnAnalyseStep(int R, int C)
{
	int32 r = R;
	int32 c = C;
	int indexlast = OutputTileIndex[c][r];
	//Up
	if (OutputTilesMaybe.IsValidIndex(c - 1) && OutputTilesMaybe[c - 1].IsValidIndex(r)
		&& OutputTilesMaybe[c - 1][r].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsUd)
		{
			if (Tile.Right == indexlast)
			{
				int32 UpTile = Tile.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[c - 1][r].Num(); i++)
				{
					if (UpTile == OutputTilesMaybe[c - 1][r][i])
					{
						IsFind = true;
						break;
					}
				}
				if (IsFind)
				{
					Neighbors.Add(UpTile);
				}
			}
		}
		OutputTilesMaybe[c - 1][r] = Neighbors;
	}
	//Down
	if (OutputTilesMaybe.IsValidIndex(c + 1) && OutputTilesMaybe[c + 1].IsValidIndex(r)
		&& OutputTilesMaybe[c + 1][r].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsUd)
		{
			if (Tile.Left == indexlast)
			{
				int32 DownTile = Tile.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[c + 1][r].Num(); i++)
				{
					if (DownTile == OutputTilesMaybe[c + 1][r][i])
					{
						IsFind = true;
						break;
					}
				}
				if (IsFind)
				{
					Neighbors.Add(DownTile);
				}
			}
		}
		OutputTilesMaybe[c + 1][r] = Neighbors;
	}
	//Left
	if (OutputTilesMaybe.IsValidIndex(c) && OutputTilesMaybe[c].IsValidIndex(r - 1)
		&& OutputTilesMaybe[c][r - 1].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsLR)
		{
			if (Tile.Right == indexlast)
			{
				int32 LeftTile = Tile.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[c][r - 1].Num(); i++)
				{
					if (LeftTile == OutputTilesMaybe[c][r - 1][i])
					{
						IsFind = true;
						break;
					}
				}
				if (IsFind)
				{
					Neighbors.Add(LeftTile);
				}
			}
		}
		OutputTilesMaybe[c][r - 1] = Neighbors;
	}
	//Right
	if (OutputTilesMaybe.IsValidIndex(c) && OutputTilesMaybe[c].IsValidIndex(r + 1)
		&& OutputTilesMaybe[c][r + 1].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsLR)
		{
			if (Tile.Left == indexlast)
			{
				int32 RightTile = Tile.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[c][r + 1].Num(); i++)
				{
					if (RightTile == OutputTilesMaybe[c][r + 1][i])
					{
						IsFind = true;
						break;
					}
				}
				if (IsFind)
				{
					Neighbors.Add(RightTile);
				}
			}
		}
		OutputTilesMaybe[c][r + 1] = Neighbors;
	}
}

FOverlapTileInfo UWFCOverlapAsset::FindTileFormArray(int r, int c,int rMax, int cMax, TArray<TArray<FPixelInfo>>& PixelArray)
{
	FOverlapTileInfo OverlapTile = FOverlapTileInfo(AnalyseSize);
	for (int i=0; i<AnalyseSize; i++)
	{
		for (int j=0; j< AnalyseSize; j++)
		{
			OverlapTile.Data[i][j] = PixelArray[(c + i)%cMax][(r + j)%rMax];
		}
	}
	return OverlapTile;
}

int32 UWFCOverlapAsset::CheckAndAddTileInAll(FOverlapTileInfo NewTile)
{
	for (int k =0;k<AllOverlapTiles.Num(); k++)
	{
		bool isFind = true;
		FOverlapTileInfo Tile = AllOverlapTiles[k];
		for (int i = 0; i < Tile.TileSize; i++)
		{
			for (int j = 0; j < Tile.TileSize; j++)
			{
				if (Tile.Data[i][j].R != (NewTile.Data)[i][j].R
					|| Tile.Data[i][j].G != (NewTile.Data)[i][j].G
					|| Tile.Data[i][j].B != (NewTile.Data)[i][j].B
					|| Tile.Data[i][j].A != (NewTile.Data)[i][j].A)
				{
					isFind = false;
				}
			}
		}
		if (isFind)
		{
			AllOverlapTiles[k].TileTimesRes = AllOverlapTiles[k].TileTimesRes + 1;
			return k;
		}
	}
	return AllOverlapTiles.Add(NewTile);
}


void UWFCOverlapAsset::AddNeighBorsLR(int32 L, int32 R)
{
	for (int c=0; c<AnalyseSize; c++)
	{
		for (int r=1; r< AnalyseSize;r++)
		{
			if (AllOverlapTiles[L].Data[c][r].R != AllOverlapTiles[R].Data[c][r -1].R
				|| AllOverlapTiles[L].Data[c][r].G != AllOverlapTiles[R].Data[c][r - 1].G
				|| AllOverlapTiles[L].Data[c][r].B != AllOverlapTiles[R].Data[c][r - 1].B
				|| AllOverlapTiles[L].Data[c][r].A != AllOverlapTiles[R].Data[c][r - 1].A)
			{
				return;
			}
		}
	}
	for (int i=0; i< AllNeighborsLR.Num(); i++)
	{
		if (L == AllNeighborsLR[i].Left && R == AllNeighborsLR[i].Right)
		{
			AllNeighborsLR[i].num += 1;
			return;
		}
	}
	AllNeighborsLR.Add(FOverlapNeighborInfo(L, R));
}

void UWFCOverlapAsset::AddNeighBorsUD(int32 U, int32 D)
{
	for (int c = 1; c < AnalyseSize; c++)
	{
		for (int r = 0; r < AnalyseSize; r++)
		{
			if (AllOverlapTiles[U].Data[c][r].R != AllOverlapTiles[D].Data[c - 1][r].R
				|| AllOverlapTiles[U].Data[c][r].G != AllOverlapTiles[D].Data[c - 1][r].G
				|| AllOverlapTiles[U].Data[c][r].B != AllOverlapTiles[D].Data[c - 1][r].B
				|| AllOverlapTiles[U].Data[c][r].A != AllOverlapTiles[D].Data[c - 1][r].A)
			{
				return;
			}
		}
	}
	for (int i = 0; i < AllNeighborsUd.Num(); i++)
	{
		if (U == AllNeighborsUd[i].Left && D == AllNeighborsUd[i].Right)
		{
			AllNeighborsUd[i].num += 1;
			return;
		}
	}
	AllNeighborsUd.Add(FOverlapNeighborInfo(U, D));
}

void UWFCOverlapAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
		//&& PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCAsset, InputRes))
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCOverlapAsset, OutputRow)
			|| PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCOverlapAsset, OutputColumn))
		{
			OutputBrush.ImageSize = FVector2D(OutputRow, OutputColumn);

			PropertyChangeOutput.Broadcast();
		}
	}
}

