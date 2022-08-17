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

	// ���� ��Ÿ�� ���

	void PlayMontageOneHandAttack();

	void PlayMontageTwoHandAttack();

	void PlayMontageGunFire();

	void PlayMontageGlaveAttack();

	// ���Ÿ� ���� Ÿ�Կ� ���� ó�� �Լ�
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
	������
	������Ʈ ���� : ���� ���� ź���� �ѷ� ��)100 , ���� �� ź�� ��
	���� ���� : ������ źâ�� ��)30
	30 / 100
	15 / 100 -> R
	if ���� �� ź�� �� < �ѷ� : (źâ�� - ���� �� ź�� ��) ���� �� �ѷ� - ������ ź�� ��
	else : �ѷ� ���� �� �ѷ� = 0
	*/

public:

	void SetEquippedWeapon(AWeapon* Weapon);

};
