// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Types/AmmoType.h"
#include "RangedWeapon.generated.h"

/**
 * 
 */

class AItemAmmo;
class USoundCue;
class UTexture2D;

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

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairTop;
	
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairBottom;

protected:

	virtual void BeginPlay() override;

	void CrosshairLineTrace(FVector& OutHit);

	void PlayFireEffect();

private:

	AItem* AmmoItem;

	// ÃÑ È¹µæ ½Ã Áö±ÞµÇ´Â Åº¾à
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> AmmoClass;

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

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GunRecoil = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute")
	bool bAutomaticFire = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bAutomaticFire"))
	float FireRate = 1.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Attribute", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float ZoomScope = 30.f;

public:

	AItem* GetAmmoItem() const;
	ERangedWeaponType GetRangedWeaponType() const;
	EAmmoType GetAmmoType() const;
	int32 GetMagazine() const;
	int32 GetLoadedAmmo() const;
	void SetLoadedAmmo(const int32 Quantity);
	float GetGunRecoil() const;
	bool GetAutomaticFire() const;
	float GetFireRate() const;
	float GetZoomScope() const;
	void ConsumeAmmo();
	virtual void SetWeaponState(const EWeaponState State) override;
};
