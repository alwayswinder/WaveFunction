// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "MyMonsterCharacter.generated.h"

UCLASS()
class WFCTEST_API AMyMonsterCharacter : public AMyCharacter
{
	GENERATED_BODY()

public:
	/*Value*/

	/*Fumction*/
	// Sets default values for this character's properties
	AMyMonsterCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

private:


};
