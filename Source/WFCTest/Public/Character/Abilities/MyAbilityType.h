// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMyAbilityInputID : uint8
{
	// 0 None
	None				UMETA(DisplayName = "None"),
	Attack				UMETA(DisplayName = "Attack"),
};