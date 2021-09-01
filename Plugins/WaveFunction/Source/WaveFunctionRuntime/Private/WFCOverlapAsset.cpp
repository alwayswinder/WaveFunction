#include "WFCOverlapAsset.h"
#include "AssetRegistry\AssetRegistryModule.h"

UWFCOverlapAsset::UWFCOverlapAsset()
{
	//����
	OutputTexture = NewObject<UTexture2D>(GetPackage(), "OutputTexture2d", RF_Public | RF_Standalone | RF_MarkAsRootSet);
	OutputTexture->AddToRoot();
	OutputTexture->PlatformData = new FTexturePlatformData();
	OutputTexture->PlatformData->SizeX = OutputRow;
	OutputTexture->PlatformData->SizeY = OutputColumn;
	//�������ظ�ʽ
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
	//����һ��uint8�����鲢ȡ��ָ��
	//������Ҫ����֮ǰ���õ����ظ�ʽ
	uint8* Pixels = new uint8[OutputRow * OutputColumn * 4];
	for (int32 y = 0; y < OutputRow; y++)
	{
		for (int32 x = 0; x < OutputColumn; x++)
		{
			int32 curPixelIndex = ((y * OutputRow) + x);
			//�����������4��ͨ����ֵ
			//������Ҫ�������ظ�ʽ��֮ǰ������PF_B8G8R8A8����ô����ͨ����˳�����BGRA
			Pixels[4 * curPixelIndex] = 100;
			Pixels[4 * curPixelIndex + 1] = 50;
			Pixels[4 * curPixelIndex + 2] = 20;
			Pixels[4 * curPixelIndex + 3] = 255;
		}
	}
	//������һ��MipMap
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	OutputTexture->PlatformData->Mips.Add(Mip);
	Mip->SizeX = OutputRow;
	Mip->SizeY = OutputColumn;

	//����Texture�������Ա��޸�
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(OutputRow * OutputColumn * 4);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * OutputRow * OutputColumn * 4);
	Mip->BulkData.Unlock();

	//ͨ�����ϲ��裬����������ݵ���ʱд��
	//ִ�����������������裬�༭���е�asset����ʾ���Ա����״̬�������ָ��Asset����ȡUTexture2D��ָ�������£�
	OutputTexture->Source.Init(OutputRow, OutputColumn, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);
	OutputTexture->UpdateResource();
	
	GetPackage()->MarkPackageDirty();
	//FAssetRegistryModule::AssetCreated(OutputTexture);
	//ͨ��asset·����ȡ�����ļ���
	//FString AssetPath = TEXT("/Game") + GetPackage()->GetPathName() + TEXT("/") + TEXT("OutputTexture2d");

	//FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
	//���б���
	//bool bSaved = UPackage::SavePackage(GetPackage(), OutputTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	delete[] Pixels;
}

