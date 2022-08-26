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

	UFUNCTION()
	void OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision;

	UPROPERTY(EditAnywhere)
	float MeleeDamage = 20.f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState = EWeaponState::EWS_MAX;

public:

	OnWeaponStateChangedDelegate WeaponStateChanged;

	EWeaponType GetWeaponType() const;
	virtual void SetWeaponState(const EWeaponState State);
	EWeaponState GetWeaponState() const;

};
