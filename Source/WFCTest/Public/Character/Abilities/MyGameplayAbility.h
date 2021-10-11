// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyAbilityType.h"
#include "MyGameplayAbility.generated.h"

UCLASS()
class UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EMyAbilityInputID AbilityInputID = EMyAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EMyAbilityInputID AbilityID = EMyAbilityInputID::None;
};