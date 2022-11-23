// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedHitScanWeapon.generated.h"

/**
 * 
 */

class ACasing;

UCLASS()
class ADVENTUREOFSHINBI_API ARangedHitScanWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:

	ARangedHitScanWeapon();

	virtual void Firing() override;

protected:

	void SpawnCasing();

	void ScatterFiring();

	void SingleFiring();

	void GetTraceStart(FVector& Start);

	FVector GetTraceEnd(const FVector& Start);

	void DrawTrailParticle(const FVector StartPoint);

	void ProcessHitResult(const FHitResult& HitResult);

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	TSubclassOf<ACasing> CasingClass;

	// 캐릭터 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* TargetHitParticle;

	// 캐릭터가 아닌 월드 액터 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* WorldHitParticle;

	// 탄 궤적
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	UParticleSystem* TrailParticle;

	// 적중 시 효과음
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	USoundCue* TargetHitSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effects")
	USoundCue* WorldHitSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BulletSpread = 0.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes")
	bool bScatter = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Attributes", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	FVector HitPoint;

	FRotator ImpactRotator;

};
