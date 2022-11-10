// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Revenant.generated.h"

/**
 * 
 */

class AAOSCharacter;

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

	virtual void Firing() override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Charging(float DeltaTime);

	void ChargeShot();

	UFUNCTION()
	void RightButtonClicking(bool bClicking);

	virtual void SetWeaponState(const EWeaponState State) override;

	void InitPartsLoc();

private:

	AAOSCharacter* WeaponOwner;

	bool bDelBinded = false;

	bool bRightButtonClicking = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant")
	TSubclassOf<AProjectile> ObliterateClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Revenant")
	UParticleSystem* MuzzleFlash2;

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

	float ChargingTime = 0.f;
};
