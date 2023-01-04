// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileHoming.generated.h"

/**
 * 유도 기능이 있는 투사체
 */

class UParticleSystem;
class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileHoming : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileHoming();

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void PlayHitEffect(const FHitResult& Hit, AActor* OtherActor);

	void PlayNoHitParticle();

	void DestroyProjectile();

	/** 근처의 적 액터를 탐색 */
	void CheckNearbyEnemy();

private:

	UPROPERTY(EditAnywhere)
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BodyParticleComp;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* BodyNiagaraComp;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	UParticleSystem* TargetHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	UParticleSystem* WorldHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	UParticleSystem* NoHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	USoundCue* FlyingSound;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	USoundCue* HitSound;

	FTimerHandle DestroyTimer;
	FTimerHandle NoHitTimer;

	UPROPERTY(EditAnywhere, Category = "Projectile | Setting")
	float DestroyTime = 1.f;

	/** 적 액터 검사 타이머 */
	FTimerHandle CheckEnemyTimer;

	/** 적 액터 검사 주기 */
	float CheckEnemyTime = 0.1f;
	
};
