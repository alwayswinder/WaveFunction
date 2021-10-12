// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MyPlayerState.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSetBase.h"
#include "MyCharacter.h"

AMyPlayerState::AMyPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSetBase = CreateDefaultSubobject<UMyAttributeSetBase>(TEXT("AttributeSetBase"));

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AMyPlayerState::HealthChanged);

		// Tag change callbacks
		//AbilitySystemComponent->RegisterGameplayTagEvent(KnockedDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGSPlayerState::KnockDownTagChanged);
	}
}

UAbilitySystemComponent* AMyPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UMyAttributeSetBase* AMyPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

void AMyPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	AMyCharacter* Pawn = Cast<AMyCharacter>(GetPawn());
	if (IsValid(Pawn) && !Pawn->IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Pawn)
		{
			Pawn->FinishDying();
		}
	}
}
