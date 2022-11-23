// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */

class UParticleSystem;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AMeleeWeapon();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual bool WeaponCapsuleTrace();

	bool GetCapsuleTraceHitResult(TArray<FHitResult>& HitResults);

	void PlayAttackEffect(const FVector& Location, const FRotator& Rotation);

	float CapsuleRadius = 0.f;

	float CapsuleHalfHeight = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Melee Weapon")
	TSubclassOf<UCameraShakeBase> CameraShakeHitImpact;

private:

	bool bActivateWeaponTrace = false;

public:

	void ActivateWeaponTrace();
	void DeactivateWeaponTrace();

};
