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
	for (int32 X = 0; X < Row; X++)
	{
		TArray<FPixelInfo> TmpPixel;
		for (int32 Y = 0; Y < Column; Y++)
		{
			TmpPixel.Add(PixelInput[Y * Row + X]);
		}
		PixelRC.Add(TmpPixel);
	}

	AllNeighborsLR.Empty();
	AllNeighborsUd.Empty();

	for (int r = 0; r<Row; r++)
	{
		for (int c = 0; c< Column; c++)
		{
			FOverlapTileInfo PixelHere = FindTileFormArray(r, c,Row, Column, PixelRC);
			FOverlapTileInfo PixelNext = FindTileFormArray(r+1, c, Row, Column, PixelRC);
			FOverlapTileInfo PixelDown = FindTileFormArray(r, c+1, Row, Column, PixelRC);

			int32 IndexL = CheckAndAddTileInAll(PixelHere);
			int32 IndexR = CheckAndAddTileInAll(PixelNext);
			int32 IndexD = CheckAndAddTileInAll(PixelDown);

			AddNeighBorsLR(IndexL, IndexR);
			AddNeighBorsUD(IndexL, IndexD);
		}
	}

	OutputTileIndex.Empty();
	OutputTilesMaybe.Empty();

	TArray<int32> AllIndexMaybe;
	for (int i=0; i<AllOverlapTiles.Num(); i++)
	{
		AllIndexMaybe.Add(i);
	}
	for (int r=0; r<OutputRow; r++)
	{
		TArray<int32> Tmp;
		TArray<TArray<int32>> TmpMaybe;
		for (int c=0; c<OutputColumn; c++)
		{
			Tmp.Add(-1);
			TmpMaybe.Add(AllIndexMaybe);
		}
		OutputTileIndex.Add(Tmp);
		OutputTilesMaybe.Add(TmpMaybe);
	}

	OutputTileIndex[0][0] = FMath::RandRange(0, AllOverlapTiles.Num() - 1);
	OutputTilesMaybe[0][0].Empty();
	NextR = 0;
	NextC = 0;
	OnAnalyseStep();

	FillOutput();
}

void UWFCOverlapAsset::ClearOutput()
{
	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
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
	while(!IsAllSet)
	{
		int32 index = FMath::RandRange(0, OutputTilesMaybe[NextR][NextC].Num() - 1);
		if (OutputTilesMaybe[NextR][NextC].IsValidIndex(index))
		{
			OutputTileIndex[NextR][NextC] = OutputTilesMaybe[NextR][NextC][index];
		}
		OutputTilesMaybe[NextR][NextC].Empty();
		OnAnalyseStep();
	}
	

	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			FOverlapTileInfo Tile(AnalyseSize);
			if (OutputTileIndex[y][x] != -1)
			{
				Tile = AllOverlapTiles[OutputTileIndex[y][x]];
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
	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
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
	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
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

	for (int32 X = 0; X < InputImage->PlatformData->SizeX; X++)
	{
		for (int32 Y = 0; Y < InputImage->PlatformData->SizeY; Y++)
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

void UWFCOverlapAsset::OnAnalyseStep()
{
	int32 r = NextR;
	int32 c = NextC;
	int indexlast = OutputTileIndex[r][c];
	//Up
	if (OutputTilesMaybe.IsValidIndex(r - 1) && OutputTilesMaybe[r - 1].IsValidIndex(c)
		&& OutputTilesMaybe[r - 1][c].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsUd)
		{
			if (Tile.Right == indexlast)
			{
				int32 UpTile = Tile.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r - 1][c].Num(); i++)
				{
					if (UpTile == OutputTilesMaybe[r - 1][c][i])
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
		OutputTilesMaybe[r - 1][c] = Neighbors;
	}
	//Down
	if (OutputTilesMaybe.IsValidIndex(r + 1) && OutputTilesMaybe[r + 1].IsValidIndex(c)
		&& OutputTilesMaybe[r + 1][c].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsUd)
		{
			if (Tile.Left == indexlast)
			{
				int32 DownTile = Tile.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r + 1][c].Num(); i++)
				{
					if (DownTile == OutputTilesMaybe[r + 1][c][i])
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
		OutputTilesMaybe[r + 1][c] = Neighbors;
	}
	//Left
	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c - 1)
		&& OutputTilesMaybe[r][c - 1].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsLR)
		{
			if (Tile.Right == indexlast)
			{
				int32 LeftTile = Tile.Left;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r][c - 1].Num(); i++)
				{
					if (LeftTile == OutputTilesMaybe[r][c - 1][i])
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
		OutputTilesMaybe[r][c - 1] = Neighbors;
	}
	//Right
	if (OutputTilesMaybe.IsValidIndex(r) && OutputTilesMaybe[r].IsValidIndex(c + 1)
		&& OutputTilesMaybe[r][c + 1].Num() > 0)
	{
		TArray<int32> Neighbors;
		for (auto Tile : AllNeighborsLR)
		{
			if (Tile.Left == indexlast)
			{
				int32 RightTile = Tile.Right;
				bool IsFind = false;
				for (int i = 0; i < OutputTilesMaybe[r][c + 1].Num(); i++)
				{
					if (RightTile == OutputTilesMaybe[r][c + 1][i])
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
		OutputTilesMaybe[r][c + 1] = Neighbors;
	}

	int MinMaybeNum = AllOverlapTiles.Num() + 1;
	IsAllSet = true;
	for (int32 R =0; R < OutputRow; R++)
	{
		for (int32 C =0; C < OutputColumn; C++)
		{
			if (OutputTilesMaybe[R][C].Num() > 0 && OutputTilesMaybe[R][C].Num() < MinMaybeNum)
			{
				MinMaybeNum = OutputTilesMaybe[R][C].Num();
				NextR = R;
				NextC = C;
			}
			if (OutputTilesMaybe[R][C].Num() > 0)
			{
				IsAllSet = false;
			}
		}
	}
}

FOverlapTileInfo UWFCOverlapAsset::FindTileFormArray(int r, int c,int rMax, int cMax, TArray<TArray<FPixelInfo>>& PixelArray)
{
	FOverlapTileInfo OverlapTile = FOverlapTileInfo(AnalyseSize);
	for (int i=0; i<AnalyseSize; i++)
	{
		for (int j=0; j< AnalyseSize; j++)
		{
			OverlapTile.Data[i][j] = PixelArray[(r + i)%rMax][(c + j)%cMax];
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
			return k;
		}
	}
	return AllOverlapTiles.Add(NewTile);
}


void UWFCOverlapAsset::AddNeighBorsLR(int32 L, int32 R)
{
	for (int i=0; i< AllNeighborsLR.Num(); i++)
	{
		if (L == AllNeighborsLR[i].Left && R == AllNeighborsLR[i].Right)
		{
			return;
		}
	}
	AllNeighborsLR.Add(FOverlapNeighborInfo(L, R));
}

void UWFCOverlapAsset::AddNeighBorsUD(int32 U, int32 D)
{
	for (int i = 0; i < AllNeighborsUd.Num(); i++)
	{
		if (U == AllNeighborsUd[i].Left && D == AllNeighborsUd[i].Right)
		{
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

