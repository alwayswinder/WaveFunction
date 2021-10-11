// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MyCharacter.generated.h"

UCLASS()
class WFCTEST_API AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "MyCharacter|Attributes")
	virtual int32 GetAbilityLevel(EMyAbilityInputID AbilityID) const;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyCharacter|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyCharacter|Abilities")
	TArray<TSubclassOf<class UMyGameplayAbility>> CharacterAbilities;

	UPROPERTY()
	class UMyAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UMyAttributeSetBase* AttributeSetBase;

	/**/
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeAttributes();

	virtual void AddCharacterAbilities();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
