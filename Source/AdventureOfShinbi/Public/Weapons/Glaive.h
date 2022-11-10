// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/MeleeWeapon.h"
#include "Glaive.generated.h"

/**
 * 
 */

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

	virtual bool WeaponCapsuleTrace() override;

	void InterpWeaponPartsRotator(float DeltaTime, bool SickleMode);

	UFUNCTION()
	void FormChange(bool bRightButtonClicked);

	UFUNCTION(BlueprintCallable)
	void CheckMana();

	UFUNCTION(BlueprintCallable)
	void EmitMagicalBall(bool bIsOpposite);

	void PlayEffect(UParticleSystem* Particle, USoundCue* Sound);

	void SpawnProjectile(FName SocketName = "MuzzleSocket");

	void ActivateSickleModeParticle();
	void DeactivateSickleModeParticle();

	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack1();

	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack2();

	void HandleTraceHitResult(TArray<FHitResult>& Results);

private:

	AAOSCharacter* WeaponOwner;

	bool bSickleMode = false;

	bool bDelBinded = false;

	bool bAbleMagicSkill = false;

	UPROPERTY(EditAnywhere, Category = "Glaive")
	float FormChangeSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Glaive")
	float SkillDamage = 50.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBladeValue = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBodyValue = FRotator::ZeroRotator;

	FRotator SickleBodyRotator = FRotator(0.f, 0.f, 180.f);
	FRotator SickleBladeRotator = FRotator(90.f, 0.f, 0.f);
	FRotator GlaiveRotator = FRotator::ZeroRotator;

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

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float EmitMagicalBallManaConsumption = 10.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateActivateParticle;

	UParticleSystemComponent* UltimateActivateParticleComp;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack1Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack2Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	USoundCue* GlaiveModeSound; // 몽타주 칼날 소리 좀 더 날카로운 칼날 소리로

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float UltimateSkillManaConsumption = 10.f;

	int8 HitStack = 0;

	FVector Start;
	FVector End;

public:

	bool GetSickleMode() const;
	bool GetHitStackIsFull() const;
	float GetUltiSkillMana() const;

};
