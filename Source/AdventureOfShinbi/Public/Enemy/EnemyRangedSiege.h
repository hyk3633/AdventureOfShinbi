// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyRangedSiege.generated.h"

/**
 * 
 */

class AProjectile;
class UAnimMontage;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyRangedSiege : public AEnemyRanged
{
	GENERATED_BODY()

public:

	AEnemyRangedSiege();

	void SiegeModeAttack();

	UFUNCTION(BlueprintCallable)
	void ConvertSiegeMode();

	UFUNCTION(BlueprintCallable)
	void ReleaseSiegeMode();

protected:

	virtual void BeginPlay() override;

	virtual void HandleStiffAndStun(FName& BoneName) override;

	void SiegeModeProjectileFire();

	void PlaySiegeModeFireMontage();

	void PlaySiegeModeHitReactionMontage();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeFireMontageEnded();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeHitReactionMontageEnded();

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> SiegeModeProjectileClass;

	UPROPERTY(EditAnywhere)
	UAnimMontage* SiegeModeFireMontage;

	UPROPERTY(EditAnywhere)
	TArray<FName> SiegeModeFireMontageSectionNameArr;

	UPROPERTY(EditAnywhere)
	UAnimMontage* SiegeModeHitReactionMontage;

	bool bSiegeMode = false;
	
};
