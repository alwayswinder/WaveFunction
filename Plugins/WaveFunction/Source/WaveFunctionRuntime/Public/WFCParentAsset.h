// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "WFCParentAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FPropertyChangeEvent)

UCLASS(BlueprintType, Blueprintable)
class WAVEFUNCTIONRUNTIME_API UWFCParentAsset : public UObject
{
	GENERATED_BODY()

public:
	virtual void BrushSizeChange(FString ChangeType = "Add") {};

	FPropertyChangeEvent PropertyChangeInput;
	FPropertyChangeEvent PropertyChangeOutput;
};
