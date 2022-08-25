// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/WeaponType.h"
#include "Types/WeaponState.h"
#include "Types/ItemRank.h"
#include "Types/ItemType.h"
#include "Weapon.generated.h"

class USphereComponent;
class UBoxComponent;
class UWidgetComponent;
class UPickupWidget;
class UTexture2D;
class AWeapon;
class UInventorySlot;

DECLARE_MULTICAST_DELEGATE_OneParam(OnWeaponStateChangedDelegate, AWeapon* Weapon);

UCLASS()
class ADVENTUREOFSHINBI_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetPickupWidgetInfo();

private:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision;

	UPROPERTY(EditAnywhere)
	float MeleeDamage = 20.f;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

	UPROPERTY(EditAnywhere)
	FString WeaponName;

	UPROPERTY(EditAnywhere)
	EItemType InfoItemType;

	UPROPERTY(EditAnywhere)
	EItemRank InfoItemRank;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* Widget;

	UPROPERTY(EditAnywhere)
	UTexture2D* WeaponIcon;

	EWeaponState WeaponState = EWeaponState::EWS_MAX;

	UInventorySlot* InventorySlot;

public:

	OnWeaponStateChangedDelegate WeaponStateChanged;

	EWeaponType GetWeaponType() const;
	USkeletalMeshComponent* GetWeaponMesh() const;
	UWidgetComponent* GetWidget() const;
	UTexture2D* GetWeaponIcon() const;
	virtual void SetWeaponState(const EWeaponState State);
	EWeaponState GetWeaponState() const;
	void SetInventorySlot(UInventorySlot* Slot);
	UInventorySlot* GetInventorySlot() const;
};
