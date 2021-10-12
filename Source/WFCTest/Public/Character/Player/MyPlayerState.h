// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "MyPlayerState.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMyOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * 
 */
UCLASS()
class WFCTEST_API AMyPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AMyPlayerState();

	virtual void BeginPlay() override;

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UMyAttributeSetBase* GetAttributeSetBase() const;
	
protected:
	FGameplayTag DeadTag;
	FDelegateHandle HealthChangedDelegateHandle;

	UPROPERTY()
	class UMyAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class UMyAttributeSetBase* AttributeSetBase;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
