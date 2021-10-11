// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MyPlayerState.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSetBase.h"

AMyPlayerState::AMyPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSetBase = CreateDefaultSubobject<UMyAttributeSetBase>(TEXT("AttributeSetBase"));

}

UAbilitySystemComponent* AMyPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UMyAttributeSetBase* AMyPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}
