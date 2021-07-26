#include "WFCAsset.h"



void UWFCAsset::InitResBase(TArray<FAssetData>& InResBase)
{
	for (auto AsserData:InResBase)
	{
		UTexture2D* Temp = Cast<UTexture2D>(AsserData.GetAsset());
		if (Temp)
		{
			ResBase.Add(Temp);
		}
	}
}

