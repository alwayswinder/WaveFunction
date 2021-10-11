// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "MyPlayerCharacter.generated.h"


UCLASS()
class WFCTEST_API AMyPlayerCharacter : public AMyCharacter
{
	GENERATED_BODY()

public:
	/*Value*/

	/*Fumction*/
	// Sets default values for this character's properties
	AMyPlayerCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	/*Value*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MyCharacter|Player|Camera")
	float BaseTurnRate;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MyCharacter|Player|Camera")
	float BaseLookUpRate;

	/*Function*/
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Mouse
	void LookUp(float Value);
	// Gamepad
	void LookUpRate(float Value);
	// Mouse
	void Turn(float Value);
	// Gamepad
	void TurnRate(float Value);
	// Mouse + Gamepad
	void MoveForward(float Value);
	// Mouse + Gamepad
	void MoveRight(float Value);

private:


};
