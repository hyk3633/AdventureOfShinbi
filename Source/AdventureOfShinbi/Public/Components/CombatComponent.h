// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AmmoType.h"
#include "CombatComponent.generated.h"

class AAOSCharacter;
class AAOSController;
class AAOSHUD;
class UAnimMontage;
class AItem;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	friend AAOSCharacter;

	UCombatComponent();

	void MeleeAttack();

	void GunFire();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PlayMontageOneHandAttack();

	void PlayMontageTwoHandAttack();

	void PlayMontageGunFire();

	void PlayMontageGlaveAttack();

	// 원거리 무기 타입에 따른 처리 함수
	void RangedWeaponFire();

	void ResetCombo();

	void Zoom(float DeltaTime);

	void InitializeAmmoMap();

	void Reload();

	// 플레이어 스탯 처리

	void UpdateHealth(float Damage);

	void UpdateStamina(float DeltaTime);

	void SetCrosshair();

	void SpreadCrosshair(float DeltaTime);

	void PickingUpItem(AItem* PickedItem);

	void WeaponQuickSwap();

	void OnChangedWeaponState(AWeapon* Weapon);

	void EquipWeapon(AWeapon* Weapon);

	void UnEquipWeapon(AWeapon* Weapon);

	void WeaponToQuickSlot1(AWeapon* Weapon);

	void WeaponToQuickSlot2(AWeapon* Weapon);

	void DiscardWeapon(AWeapon* Weapon);

private:

	AAOSCharacter* Character;

	AAOSController* CharacterController;

	AAOSHUD* HUD;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float Mana = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxMana = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats", meta = (ClampMin = "10.0", ClampMax = "500.0"))
	float MaxStamina = 100.f;

	float StaminaDecreaseRate = 20.f;

	float StaminaIncreaseRate = 20.f;

	AItem* EquippedItem = nullptr;

	AWeapon* EquippedWeapon = nullptr;

	AWeapon* QuickSlot1Weapon = nullptr;
	AWeapon* QuickSlot2Weapon = nullptr;

	int8 MeleeAttackComboCount = 0;
	int8 RandomMontageNum;

	FTimerHandle ComboTimer;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ComboTime = 2.0f;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeOneHandAttackMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MeleeTwoHandAttackMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* GunFireMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* GlaveAttackMontage;

	bool IsAnimationPlaying = false;

	bool bCanRunning = true;

	float DefaultFOV = 0.f;

	float CurrentFOV = 0.f;

	// 값을 저장해서 쓸건지?
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 50.f;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 30.f;

	TMap<EAmmoType, int32> AmmoMap;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;

	TArray<AWeapon*> AcquiredWeapons;

};
