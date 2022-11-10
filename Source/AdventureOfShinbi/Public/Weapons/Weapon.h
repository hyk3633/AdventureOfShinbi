// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/WeaponType.h"
#include "Types/WeaponState.h"
#include "Weapon.generated.h"

class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(OnWeaponStateChangedDelegate, AWeapon* Weapon);

UCLASS()
class ADVENTUREOFSHINBI_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

private:

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState = EWeaponState::EWS_MAX;

public:

	OnWeaponStateChangedDelegate WeaponStateChanged;

	EWeaponType GetWeaponType() const;
	virtual void SetWeaponState(const EWeaponState State);
	EWeaponState GetWeaponState() const;
	float GetWeaponDamage();
};
