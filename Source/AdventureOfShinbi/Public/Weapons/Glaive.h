// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/MeleeWeapon.h"
#include "Glaive.generated.h"

class AAOSCharacter;
class AProjectile;
class UParticleSystem;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AGlaive : public AMeleeWeapon
{
	GENERATED_BODY()
	
public:

	AGlaive();

	virtual void SetWeaponState(const EWeaponState State) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** ������ ĸ�� Ʈ���̽� ����� ���� ������ ���� �� ȿ�� ��� */
	virtual bool WeaponCapsuleTrace() override;

	/** ���� ���̷�Ż �޽��� �� ��ġ ���� ���� */
	void InterpWeaponPartsRotator(float DeltaTime, bool SickleMode);

	/** 
	* ������ ��忡 ���� �� ����
	* @param bRightButtonClicked ���콺 ��Ŭ���� ������ true
	*/
	UFUNCTION()
	void FormChange(bool bRightButtonClicked);

	/** ������ �Ҹ� �̻� �����ִ��� Ȯ�� */
	UFUNCTION(BlueprintCallable)
	void CheckMana();

	/** ����ü �߻� 
	* @param �ִϸ��̼� �󿡼� ���� ��ġ�� �ٴڿ� ������ true 
	*/
	UFUNCTION(BlueprintCallable)
	void EmitMagicalBall(bool bIsOpposite);

	void PlayEffect(UParticleSystem* Particle, USoundCue* Sound);

	void SpawnProjectile(FName SocketName = "MuzzleSocket");

	void ActivateSickleModeParticle();
	void DeactivateSickleModeParticle();

	void ActivateUltimateParticle();

	/** ��ų : ������ �ټ��� ���� �������� ���� �������� ���� CapsuleTrace ��� */
	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack1();

	/** ��ų : ������ �ټ��� ���� �������� ���� �������� ���� SphereTrace ��� */
	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack2();

	void PlayCameraShake();

	/** Trace ����� ���� ������ ����, ����Ʈ ���, LaunchCharacter ���� ���� */
	void HandleTraceHitResult(TArray<FHitResult>& Results);

private:

	AAOSCharacter* WeaponOwner;

	/** ��� ��� ���� */
	bool bSickleMode = false;

	/** ��������Ʈ�� ���ε� ���� */
	bool bDelBinded = false;

	/** ��ų ��� ���ɿ��� */
	bool bAbleMagicSkill = false;

	/** ���� �޽��� �� ȸ�� �ӵ� */
	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	float FormChangeSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	float SkillDamage = 1250.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	TSubclassOf<UCameraShakeBase> CameraShakeUltimate;

	/** ������ ���� �޽��� Į�� �� Rotation */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBladeValue = FRotator::ZeroRotator;

	/** ������ ���� �޽��� ��ü �� Rotation */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBodyValue = FRotator::ZeroRotator;

	/** �� ����� Rotation �� */
	FRotator SickleBodyRotator = FRotator(0.f, 0.f, 180.f);
	FRotator SickleBladeRotator = FRotator(90.f, 0.f, 0.f);

	/** ���� �޽��� �� Rotation �⺻ �� */
	FRotator GlaiveRotator = FRotator::ZeroRotator;

	/** ����ü ���ø� Ŭ���� */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* SickleModeParticle;

	UParticleSystemComponent* SickleModeParticleComp;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* FormChangeParticle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* MuzzlFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	USoundCue* SickleModeSound;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	USoundCue* EmitSound;

	/** ����ü �߻� ���� �Ҹ� */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float EmitMagicalBallManaConsumption = 20.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateActivateParticle;

	UParticleSystemComponent* UltimateActivateParticleComp;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack1Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack2Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	USoundCue* GlaiveModeSound;

	/** ��ų ���� �Ҹ� */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float UltimateSkillManaConsumption = 100.f;

	/** Ÿ�� Ƚ�� */
	int8 HitStack = 0;

	/** �ִ� Ÿ�� Ƚ�� */
	int8 MaxHitStack = 5;

public:

	bool GetSickleMode() const;
	bool GetHitStackIsFull() const;
	float GetUltiSkillMana() const;

};
