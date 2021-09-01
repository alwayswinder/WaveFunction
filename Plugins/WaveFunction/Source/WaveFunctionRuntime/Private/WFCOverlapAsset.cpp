#include "WFCOverlapAsset.h"
#include "AssetRegistry\AssetRegistryModule.h"

UWFCOverlapAsset::UWFCOverlapAsset()
{
	//创建
	OutputTexture = NewObject<UTexture2D>(GetPackage(), "OutputTexture2d", RF_Public | RF_Standalone | RF_MarkAsRootSet);
	OutputTexture->AddToRoot();
	OutputTexture->PlatformData = new FTexturePlatformData();
	OutputTexture->PlatformData->SizeX = OutputRow;
	OutputTexture->PlatformData->SizeY = OutputColumn;
	//设置像素格式
	OutputTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

	OutputBrush.SetResourceObject(OutputTexture);
	OutputBrush.ImageSize = FVector2D(OutputRow, OutputColumn);
}

void UWFCOverlapAsset::Analyse()
{

}

void UWFCOverlapAsset::ClearOutput()
{

}

void UWFCOverlapAsset::FillOutput()
{
	WriteToTexture();
}

void UWFCOverlapAsset::WriteToTexture()
{
	//创建一个uint8的数组并取得指针
	//这里需要考虑之前设置的像素格式
	uint8* Pixels = new uint8[OutputRow * OutputColumn * 4];
	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			//这里可以设置4个通道的值
			//这里需要考虑像素格式，之前设置了PF_B8G8R8A8，那么这里通道的顺序就是BGRA
			Pixels[4 * curPixelIndex] = 100;
			Pixels[4 * curPixelIndex + 1] = 50;
			Pixels[4 * curPixelIndex + 2] = 20;
			Pixels[4 * curPixelIndex + 3] = 255;
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

