// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyStrafing/EnemyStrafingAnimInstance.h"
#include "EnemyBossAnimInstance.generated.h"

/**
 * 
 */

class AEnemyBoss;

UCLASS()
class ADVENTUREOFSHINBI_API UEnemyBossAnimInstance : public UEnemyStrafingAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void UpdateAnimationProperties(float DeltaTime) override;

	virtual void NativeInitializeAnimation() override;

private:

	UPROPERTY()
	AEnemyBoss* Boss;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bPhase2;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bStrafing;
};
