// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedWeapon.h"
#include "RangedProjectileWeapon.generated.h"

/**
 * 
 */

class AProjectile;

UCLASS()
class ADVENTUREOFSHINBI_API ARangedProjectileWeapon : public ARangedWeapon
{
	GENERATED_BODY()
	
public:

	virtual void Firing() override;

	void ScatterFiring(TSubclassOf<AProjectile> Projectile);

	void SingleFiring(TSubclassOf<AProjectile> Projectile);

protected:

	void SpawnProjectile(TSubclassOf<AProjectile> Projectile, const FVector& LocToSpawn, const FRotator& RotToSpawn);

	void GetSpawnLocation(FVector& ProjLoc);

	void GetSpawnRotation(const FVector& ProjLoc, FRotator& ProjRot);

protected:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter")
	bool bScatterGun = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Scatter", meta = (EditCondition = "bScatterGun", ClampMin = "0.1", ClampMax = "10.0"))
	float ScatterRange = 1.0f;

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

};
