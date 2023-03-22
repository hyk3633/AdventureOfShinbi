
#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Types/AmmoType.h"
#include "Types/WeaponType.h"
#include "RangedWeapon.generated.h"

/**
 * 원거리 무기
 */

class AItemAmmo;
class USoundCue;
class UTexture2D;

// 크로스헤어 구조체
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

	/** 발사 함수 */
	virtual void Firing();

protected:

	virtual void BeginPlay() override;

	/** 화면 정중앙에서 라인 트레이스 수행 */
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

	/** 자동 발사 여부 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status")
	bool bAutomaticFire = false;

	/** 연사력 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status", meta = (ClampMin = "0.0", ClampMax = "5.0", EditCondition = "bAutomaticFire"))
	float FireRate = 1.f;

	/** 총기 반동 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Status", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GunRecoil = 0.35f;

private:

	UPROPERTY()
	AItem* AmmoItem;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> AmmoClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Crosshairs")
	FCrosshairs Crosshairs;

protected:

	/** 무기의 탄약 유형 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info")
	EAmmoType AmmoType = EAmmoType::EAT_MAX;

	/** 정조준 시 확대 배율 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float ZoomScope = 30.f;

	/** 탄창의 탄약량 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info" , meta = (ClampMin = "1", ClampMax = "1000"))
	int32 Magazine = 30;
	
	/** 장전된 탄약량 */
	UPROPERTY(VisibleAnywhere, Category = "Ranged Weapon | Info")
	int32 LoadedAmmo = 0;

	/** 탄약 소모량 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Info")
	int32 AmmoConsumption = 1;

public:

	AItem* GetAmmoItem() const;
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
