// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Wraith.generated.h"

/**
 * 
 */

class AAOSCharacter;

USTRUCT(BlueprintType)
struct FWeaponPartsLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Barrel;							 
	UPROPERTY(BlueprintReadWrite)
	FVector Scope;							 
	UPROPERTY(BlueprintReadWrite)
	FVector Lens;							 
	UPROPERTY(BlueprintReadWrite)
	FVector StockBott;						 
	UPROPERTY(BlueprintReadWrite)
	FRotator StockBottRot;					 
	UPROPERTY(BlueprintReadWrite)
	FVector StockTop;
};

UCLASS()
class ADVENTUREOFSHINBI_API AWraith : public ARangedProjectileWeapon
{
	GENERATED_BODY()
	
public:

	AWraith();

	virtual void Firing() override;

	virtual void SetWeaponState(const EWeaponState State) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void InterpWeaponPartsLocation(float DeltaTime, bool bSniperMode);

	UFUNCTION()
	void FormChange(bool bSniperMode);

	void ActivateScopeParticle();

private:

	AAOSCharacter* WeaponOwner;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	TSubclassOf<UCameraShakeBase> CameraShakeAimingShot;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	TSubclassOf<AProjectile> AimingProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedMuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedSmokeParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedStabilizerParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* AimedFireSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* ScopeParticle;

	UParticleSystemComponent* ScopeParticleComp;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* ActivateSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* DeactivateSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAimed = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	float FormChangeSpeed = 5.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponPartsLocation CurrentLocation;

	FWeaponPartsLocation AssaultMode;

	FWeaponPartsLocation SniperMode;

	bool bDelBinded = false;
};
