// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyRanged.generated.h"

/**
 * 
 */

class AProjectile;
class UAnimMontage;
class AEnemyAIController;
class UParticleSystem;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyRanged : public AEnemyCharacter
{
	GENERATED_BODY()

public:

	AEnemyRanged();

	void RangedAttack();

protected:

	virtual void BeginPlay() override;

	void ProjectileFire();

	void CrosshairLineTrace(FVector& OutHitPoint);

	void PlayFireMontage();

	UFUNCTION(BlueprintCallable)
	void OnFireMontageEnded();

	void FinishFire();

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Option")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Option", meta = (ClampMin=0.f))
	float BulletSpread;

	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<FName> FireMontageSectionNameArr;

	UPROPERTY(EditAnywhere, Category = "Ranged Option", meta = (ClampMin = 0.f))
	uint8 FireCount = 3;

	uint8 CurrentFireCount = 0;

public:

	AEnemyAIController* GetEnemyController() const;

};
