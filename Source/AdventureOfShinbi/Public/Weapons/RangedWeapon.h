// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "RangedWeapon.generated.h"

/**
 * 
 */

class USoundCue;

UENUM(BlueprintType)
enum class ERangedWeaponType : uint8
{
	ERWT_HitScan UMETA(DisplayName = "HitScan"),
	ERWT_Projectile UMETA(DisplayName = "Projectile"),

	ERWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ADVENTUREOFSHINBI_API ARangedWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	ARangedWeapon();

protected:

	virtual void BeginPlay() override;

	virtual void Firing();

	void PlayFireEffect();

protected:

	FVector TraceHitEndPoint;

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	UParticleSystem* MuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	ERangedWeaponType RangedWeaponType = ERangedWeaponType::ERWT_MAX;

public:

	FORCEINLINE ERangedWeaponType GetRangedWeaponType() const { return RangedWeaponType; }
};
