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

protected:

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

};
