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

	virtual void Tick(float DeltaTime) override;

	virtual void HandleStiffAndStun(FName& BoneName) override;

	void SiegeModeProjectileFire();

	void PlaySiegeModeFireMontage();

	void PlaySiegeModeHitReactionMontage();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeFireMontageEnded();

	UFUNCTION(BlueprintCallable)
	void OnSiegeModeHitReactionMontageEnded();

private:

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	TSubclassOf<AProjectile> SiegeModeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	UAnimMontage* SiegeModeFireMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	TArray<FName> SiegeModeFireMontageSectionNameArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Siege Mode")
	UAnimMontage* SiegeModeHitReactionMontage;

	bool bSiegeMode = false;

public:

	bool GetSiegeMode() const;
	
};
