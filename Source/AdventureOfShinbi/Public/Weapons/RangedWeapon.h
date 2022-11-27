
#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Types/AmmoType.h"
#include "Types/WeaponType.h"
#include "RangedWeapon.generated.h"

/**
 * 
 */

class AItemAmmo;
class USoundCue;
class UTexture2D;

USTRUCT(BlueprintType)
struct FCrosshairs
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairBottom;

};

UCLASS()
class ADVENTUREOFSHINBI_API ARangedWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	ARangedWeapon();

	virtual void Firing();

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairTop;
	
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	UTexture2D* CrosshairBottom;

protected:

	virtual void BeginPlay() override;

	void CrosshairLineTrace(FVector& OutHit);

	void PlayFireEffect(UParticleSystem* Particle, USoundCue* Sound);

protected:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effect")
	UParticleSystem* MuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effect")
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effect")
	TSubclassOf<UCameraShakeBase> CameraShakeGunFire;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Effect")
	USoundCue* NoAmmoSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status")
	bool bAutomaticFire = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status", meta = (ClampMin = "0.0", ClampMax = "5.0", EditCondition = "bAutomaticFire"))
	float FireRate = 1.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GunRecoil = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info")
	ERangedWeaponType RangedWeaponType = ERangedWeaponType::ERWT_MAX;

private:

	AItem* AmmoItem;

	// ÃÑ È¹µæ ½Ã Áö±ÞµÇ´Â Åº¾à
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> AmmoClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status", meta = (ClampMin = "1", ClampMax = "100"))
	int8 DamageMultiplier = 5;

	FVector2D ViewPortSize;

	FCrosshairs Crosshairs;

protected:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info")
	EAmmoType AmmoType = EAmmoType::EAT_MAX;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float ZoomScope = 30.f;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info" , meta = (ClampMin = "1", ClampMax = "1000"))
	int32 Magazine = 30;
	
	UPROPERTY(VisibleAnywhere, Category = "Ranged Weapon | Info")
	int32 LoadedAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info")
	int32 AmmoConsumption = 1;

public:

	AItem* GetAmmoItem() const;
	ERangedWeaponType GetRangedWeaponType() const;
	float GetHeadShotDamage();
	EAmmoType GetAmmoType() const;
	int32 GetMagazine() const;
	int32 GetLoadedAmmo() const;
	void SetLoadedAmmo(const int32 Quantity);
	float GetGunRecoil() const;
	bool GetAutomaticFire() const;
	float GetFireRate() const;
	float GetZoomScope() const;
	void ConsumeAmmo();
	void PlayNoAmmoSound();
	FCrosshairs GetCrosshairs() const;
};
