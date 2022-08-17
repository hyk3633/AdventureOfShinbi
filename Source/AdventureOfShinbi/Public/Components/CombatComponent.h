// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/AmmoType.h"
#include "CombatComponent.generated.h"

class AAOSCharacter;
class AAOSController;
class AWeapon;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	friend AAOSCharacter;

	UCombatComponent();

	void Attack();

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 공격 몽타주 재생

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

private:

	AAOSCharacter* Character;

	AAOSController* CharacterController;

	AWeapon* EquippedWeapon;

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

	float DefaultFOV = 0.f;

	float CurrentFOV = 0.f;

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 50.f;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 30.f;

	TMap<EAmmoType, int32> AmmoMap;

	/*
	재장전
	컴포넌트 변수 : 내가 가진 탄약의 총량 예)100 , 장전 된 탄약 수
	무기 변수 : 무기의 탄창량 예)30
	30 / 100
	15 / 100 -> R
	if 장전 할 탄약 수 < 총량 : (탄창량 - 장전 된 탄약 수) 장전 후 총량 - 장전할 탄약 수
	else : 총량 장전 후 총량 = 0
	*/

public:

	void SetEquippedWeapon(AWeapon* Weapon);

};
