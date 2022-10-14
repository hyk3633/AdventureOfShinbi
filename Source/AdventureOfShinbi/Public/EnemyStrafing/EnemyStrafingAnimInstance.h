// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAnimInstance.h"
#include "EnemyStrafingAnimInstance.generated.h"

/**
 * 
 */

class AEnemyStrafing;

UCLASS()
class ADVENTUREOFSHINBI_API UEnemyStrafingAnimInstance : public UEnemyAnimInstance
{
	GENERATED_BODY()

public:

	virtual void UpdateAnimationProperties(float DeltaTime) override;

	virtual void NativeInitializeAnimation() override;

private:

	AEnemyStrafing* EnemyStrafingCharacter;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StrafingValue;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float JogAnimRate;
};
