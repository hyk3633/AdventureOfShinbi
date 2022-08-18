// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Types/AmmoType.h"
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

	void CrosshairLineTrace(FVector& OutHitPoint);

	void PlayFireEffect();

private:

	FVector2D ViewPortSize;

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	UParticleSystem* MuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Effect")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	ERangedWeaponType RangedWeaponType = ERangedWeaponType::ERWT_MAX;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	EAmmoType AmmoType = EAmmoType::EAT_MAX;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute" , meta = (ClampMin = "20", ClampMax = "1000"))
	int32 Magazine = 30;
	
	UPROPERTY(VisibleAnywhere, Category = "Ranged Attribute")
	int32 LoadedAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	int32 AmmoConsumption = 1;

public:

	FORCEINLINE ERangedWeaponType GetRangedWeaponType() const { return RangedWeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetMagazine() const { return Magazine; }
	FORCEINLINE int32 GetLoadedAmmo() const { return LoadedAmmo; }
	FORCEINLINE void SetLoadedAmmo(int32 Quantity) { LoadedAmmo = Quantity; }
	void ConsumeAmmo();
};