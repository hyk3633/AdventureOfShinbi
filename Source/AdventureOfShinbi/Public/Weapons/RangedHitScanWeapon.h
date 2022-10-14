// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedHitScanWeapon.generated.h"

/**
 * 
 */

UCLASS()
class ADVENTUREOFSHINBI_API ARangedHitScanWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:

	void Firing();

protected:

	void PlayAfterFireEffect(const bool EnemyHit, const bool HitAnything);

private:

	// 캐릭터 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* TargetHitParticle;

	// 캐릭터가 아닌 월드 액터 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* WorldHitParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* TrailParticle;

	// 적중 시 효과음
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BulletSpread = 0.f;

	FVector HitPoint;

	FRotator ImpactRotator;

};
