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

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyRanged : public AEnemyCharacter
{
	GENERATED_BODY()

public:

	AEnemyRanged();

	virtual void Attack() override;

	void RangedAttack();

protected:

	virtual void BeginPlay() override;

	virtual void OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnChaseRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void ProjectileFire();

	void CrosshairLineTrace(FVector& OutHitPoint);

	void PlayFireMontage();

	UFUNCTION()
	void OnFireMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, meta = (ClampMin=0.f))
	float BulletSpread;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere)
	TArray<FName> FireMontageSectionNameArr;
	
	UPROPERTY(VisibleAnywhere)
	bool bTargetIsInChaseRange = false;
	UPROPERTY(VisibleAnywhere)
	bool bTargetIsInAttackRange = false;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f))
	uint8 FireCount = 3;

	uint8 CurrentFireCount = 0;
};
