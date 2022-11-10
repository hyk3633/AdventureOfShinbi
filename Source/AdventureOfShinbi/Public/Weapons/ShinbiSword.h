// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/MeleeWeapon.h"
#include "ShinbiSword.generated.h"

/**
 * 
 */

class AProjectile;
class AProjectileShinbiWolf;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class UAudioComponent;
class AAOSCharacter;

UCLASS()
class ADVENTUREOFSHINBI_API AShinbiSword : public AMeleeWeapon
{
	GENERATED_BODY()
	
public:

	AShinbiSword();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetWeaponState(const EWeaponState State) override;

	void CirclingWolvesOn(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void WolfAttack();

	UFUNCTION(BlueprintCallable)
	void CirclingWolves();

	void PlayCirclingWolvesEffect();

	void CirclingWolvesDurationEnd();

	UFUNCTION(BlueprintCallable)
	void UltimateWolfRush();

	void PlayExplosionEffect();

	void SpawnWolf();

	void GetRandomSpawnPosition(FVector& SpawnLoc, FRotator& SpawnRot);

private:

	AAOSCharacter* WeaponOwner;

	/** 울프 어택 */
	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	TSubclassOf<AProjectile> WolfProjClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	USoundCue* WolfAttackSound;

	AProjectileShinbiWolf* WolfProj;

	/** 써클링 울브즈 */

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	UParticleSystem* CirclingParticle;

	UParticleSystemComponent* CirclingParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	UParticleSystem* LensFlareParticle;

	UParticleSystemComponent* LensFlareParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	USoundCue* CirclingWolvesSound;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	USoundCue* CirclingLoopingSound;

	UAudioComponent* CirclingLoopingSoundComp;

	TArray<AProjectileShinbiWolf*> CirclingWolvesArr;
	TArray<float> AngleAxis;
	TArray<float> AngleOffset;

	bool bCirclingWolvesOn = false;

	float Multiplier = -150.f;
	float RotateSpeed = -150.f;

	FTimerHandle CirclingWolvesDurationTimer;
	float CirclingWolvesDurationTime = 10.f;

	/** 얼티메이트 울프 */

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* PortalParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* ExplosionParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* LightParticle;

	UParticleSystemComponent* LightParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* LightEndParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	USoundCue* PortalSound;

	int8 CurrentTargetIdx = 0;

	int8 CurrentAttackCycle = 0;

	int8 MaxAttackCycle = 3;

	FTimerHandle ExplosionTimer;

	FTimerHandle UltimateWolfAttackTimer;
	float UltimateWolfAttackTime = 0.3f;

	TArray<AActor*> Enemies;
};
