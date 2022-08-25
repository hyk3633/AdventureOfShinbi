// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */


UCLASS()
class ADVENTUREOFSHINBI_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AMeleeWeapon();

protected:

	virtual void BeginPlay() override;

private:

public:

};
