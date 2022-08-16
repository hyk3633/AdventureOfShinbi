// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AAOSCharacter;
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


private:

	AAOSCharacter* Character;

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

public:

	void SetEquippedWeapon(AWeapon* Weapon);

};
