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

	void Firing();

	void ScatterFiring();

protected:

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Scatter Attribute")
	bool bScatterGun = false;

	UPROPERTY(EditAnywhere, Category = "Ranged Scatter Attribute", meta = (EditCondition = "bScatterGun", ClampMin = "1", ClampMax = "50"))
	int8 NumberOfShots = 10;

	TArray<FRotator> ShotRotator;

	UPROPERTY(EditAnywhere, Category = "Ranged Scatter Attribute", meta = (EditCondition = "bScatterGun", ClampMin = "0.1", ClampMax = "10.0"))
	float ScatterRange = 1.0f;

public:

	bool GetScatterGun() const;

};
