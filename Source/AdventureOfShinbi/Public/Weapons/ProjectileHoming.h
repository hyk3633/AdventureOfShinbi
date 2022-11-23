// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileHoming.generated.h"

/**
 * 
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

	void CheckNearbyEnemy();

private:

	UPROPERTY(EditAnywhere)
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BodyParticleComp;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* BodyNiagaraComp;

	// Ÿ�� ���� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	UParticleSystem* TargetHitParticle;

	// Ÿ���� �ƴ� ��ü ���� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Projectile | Effect")
	UParticleSystem* WorldHitParticle;

	// ������ �������� ���� ����� ��ƼŬ
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

	FTimerHandle CheckEnemyTimer;
	float CheckEnemyTime = 0.1f;
	
};
