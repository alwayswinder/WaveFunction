// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyPlayerState.h"
#include "MyAbilitySystemComponent.h"
#include "MyAttributeSetBase.h"
#include "MyAbilityType.h"

// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	bASCInputBound = false;
}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyPlayerCharacter::LookUp(float Value)
{
	if (IsAlive())
	{
		AddControllerPitchInput(Value);
	}
}

void AMyPlayerCharacter::LookUpRate(float Value)
{
	if (IsAlive())
	{
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
	}
}

void AMyPlayerCharacter::Turn(float Value)
{
	if (IsAlive())
	{
		AddControllerYawInput(Value);
	}
}

void AMyPlayerCharacter::TurnRate(float Value)
{
	if (IsAlive())
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
	}
}

void AMyPlayerCharacter::MoveForward(float Value)
{
	if (IsAlive() && !BlockMove)
	{
		AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
	}
}

void AMyPlayerCharacter::MoveRight(float Value)
{
	if (IsAlive() && !BlockMove)
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
	}
}

void AMyPlayerCharacter::BindASCInput()
{
	if (!bASCInputBound && IsValid(AbilitySystemComponent) && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EMyAbilityInputID"), static_cast<int32>(EMyAbilityInputID::Confirm), static_cast<int32>(EMyAbilityInputID::Cancel)));

		bASCInputBound = true;
	}
}

// Called every frame
void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyPlayerCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyPlayerCharacter::TurnRate);

	BindASCInput();
}


