// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Revenant.generated.h"

/**
 * 
 */

class AAOSCharacter;

/** 무기 스켈레탈 메쉬의 본 Location, Rotation 구조체 */
USTRUCT(BlueprintType)
struct FRevenentPartsLocation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float BarrelPitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float BBL = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float WBL = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float BBR = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float WBR = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float BTL = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float WTL = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float BTR = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	float WTR = 0.f;
};

UCLASS()
class ADVENTUREOFSHINBI_API ARevenant : public ARangedProjectileWeapon
{
	GENERATED_BODY()
	
public:

	ARevenant();

	/** 발사 함수 */
	virtual void Firing() override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 스켈레탈 메쉬의 본 위치 업데이트 */
	void Charging(float DeltaTime);

	/** 우클릭이 2초 유지되면 더 강력한 투사체 발사 */
	void ChargeShot();

	/** 우클릭이 활성화되는 동안 사운드 재생 */
	UFUNCTION()
	void RightButtonClicking(bool bClicking);

	virtual void SetWeaponState(const EWeaponState State) override;

	/** 스켈레탈 메쉬의 본 위치 초기화 */
	void InitPartsLoc();

private:

	/** 우클릭 여부 */
	bool bRightButtonClicking = false;

	/** 추가 투사체 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Normal Shot")
	TSubclassOf<AProjectile> ObliterateClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Normal Shot")
	UParticleSystem* MuzzleFlash2;

	/** 더 강력한 투사체 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	TSubclassOf<AProjectile> ChargeShotPorjClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	UParticleSystem* ChargeShotMuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	UParticleSystem* ChargeShotMuzzleFlash2;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	UParticleSystem* ChargingParticle;

	UParticleSystemComponent* ChargingParticleComp;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	USoundCue* ChargingLoopSound;

	UAudioComponent* ChargingLoopSoundComp;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant | Charge Shot")
	USoundCue* ChargeShotSound;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRevenentPartsLocation CurrentPartsLoc;

	FRevenentPartsLocation DeltaLoc;

	/** 누적 충전 시간 */
	float ChargingTime = 0.f;
};
