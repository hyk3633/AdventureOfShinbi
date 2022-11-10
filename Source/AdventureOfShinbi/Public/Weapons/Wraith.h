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
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	FVector Barrel;							 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	FVector Scope;							 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	FVector Lens;							 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	FVector StockBott;						 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
	FRotator StockBottRot;					 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Bone")
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

	void InitializingWeaponPartsLocation();

private:

	AAOSCharacter* WeaponOwner;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	TSubclassOf<AProjectile> AimingProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	UParticleSystem* AimedMuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	UParticleSystem* AimedSmokeParticle;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	UParticleSystem* AimedStabilizerParticle;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	USoundCue* AimedFireSound;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	UParticleSystem* ScopeParticle;

	UParticleSystemComponent* ScopeParticleComp;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	USoundCue* ActivateSound;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	USoundCue* DeactivateSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAimed = false;

	UPROPERTY(EditAnywhere, Category = "Aimed")
	float FormChangeSpeed = 5.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponPartsLocation CurrentValue;

	FWeaponPartsLocation AssaultMode;

	FWeaponPartsLocation SniperMode;

	bool bDelBinded = false;
};
