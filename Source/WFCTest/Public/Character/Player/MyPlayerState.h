// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WFCTEST_API AMyPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AMyPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UMyAttributeSetBase* GetAttributeSetBase() const;


protected:
	UPROPERTY()
	class UMyAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class UMyAttributeSetBase* AttributeSetBase;
};
