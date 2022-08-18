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

	void PlayAfterFireEffect();

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	UParticleSystem* TrailParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	USoundCue* ImpactSound;

	FVector HitPoint;

	FRotator ImpactRotator;
	
};
