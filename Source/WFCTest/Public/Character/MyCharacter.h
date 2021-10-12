// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "MyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AMyCharacter*, Character);

UCLASS()
class WFCTEST_API AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	virtual int32 GetAbilityLevel(EMyAbilityInputID AbilityID) const;

	virtual void RemoveCharacterAbilities();

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "MyCharacter")
	virtual void FinishDying();

	UPROPERTY(BlueprintAssignable, Category = "MyCharacter")
	FCharacterDiedDelegate OnCharacterDied;

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;
	bool BlockMove;
	FGameplayTag BlockMoveTag;
	FDelegateHandle BlockMoveDelegateHandle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyCharacter|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyCharacter|Abilities")
	TArray<TSubclassOf<class UMyGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MyCharacter|Abilities")
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyCharacter|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY()
	class UMyAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UMyAttributeSetBase* AttributeSetBase;

	/**/
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeAttributes();

	virtual void AddCharacterAbilities();

	virtual void BlockMoveTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	virtual void SetHealth(float Health);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
