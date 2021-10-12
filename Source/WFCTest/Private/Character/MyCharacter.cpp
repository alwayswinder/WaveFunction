// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSetBase.h"
#include "MyGameplayAbility.h"
#include "MyPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag("Effect.RemoveOnDeath");

	BlockMove = false;
	BlockMoveTag = FGameplayTag::RequestGameplayTag(FName("Ability.BlockMove"));
}

class UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AMyPlayerState* PS = GetPlayerState<AMyPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UMyAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		BlockMoveDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(BlockMoveTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AMyCharacter::BlockMoveTagChanged);

		AttributeSetBase = PS->GetAttributeSetBase();

		InitializeAttributes();

		AddCharacterAbilities();

		if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
		{
			// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
			SetHealth(GetMaxHealth());
		}
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));

	}
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

float AMyCharacter::GetMaxHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxHealth();
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

void AMyCharacter::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void AMyCharacter::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);
	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AMyCharacter::FinishDying()
{
	Destroy();
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

void AMyCharacter::BlockMoveTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (CallbackTag == BlockMoveTag)
	{
		if (NewCount > 0)
		{
			BlockMove = true;
		}
		else if (NewCount < 1)
		{
			BlockMove = false;
		}
	}
}

void AMyCharacter::SetHealth(float Health)
{
	if (IsValid(AttributeSetBase))
	{
		AttributeSetBase->SetHealth(Health);
	}
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

