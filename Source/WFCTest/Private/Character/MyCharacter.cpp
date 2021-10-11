// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSetBase.h"
#include "MyGameplayAbility.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

class UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AMyCharacter::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float AMyCharacter::GetHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

int32 AMyCharacter::GetCharacterLevel() const
{
	return 1;
}

int32 AMyCharacter::GetAbilityLevel(EMyAbilityInputID AbilityID) const
{
	return 1;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyCharacter::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}
	if (!DefaultAttributes)
	{
		return;
	}
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}

void AMyCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UMyGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

