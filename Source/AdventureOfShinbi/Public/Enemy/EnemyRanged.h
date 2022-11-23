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

	virtual void RangedAttack();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void AbortAttack() override;

	void ProjectileFire(TSubclassOf<AProjectile> Projectile);

	void CrosshairLineTrace(FVector& OutHitPoint);

	virtual void PlayFireMontage();

	UFUNCTION(BlueprintCallable)
	virtual void OnFireMontageEnded();

	void FinishFire();

	void AfterFireDelay();

protected:

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	UAnimMontage* FireMontage;

private:

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	TArray<FName> FireMontageSectionNameArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	float BulletSpread;

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack", meta = (ClampMin = 0.f))
	uint8 FireCount = 3;

	uint8 CurrentFireCount = 0;

	FTimerHandle FireDelayTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Ranged Attack")
	float FireDelayTime = 0.1f;

public:

	AEnemyAIController* GetEnemyController() const;

};
