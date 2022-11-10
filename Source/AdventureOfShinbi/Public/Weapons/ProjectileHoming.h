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

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystemComponent* BodyParticleComp;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* BodyNiagaraComp;

	// Ÿ�� ���� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* TargetHitParticle;

	// Ÿ���� �ƴ� ��ü ���� �� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* WorldHitParticle;

	// ������ �������� ���� ����� ��ƼŬ
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* NoHitParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* FlyingSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* HitSound;

	FTimerHandle DestroyTimer;
	FTimerHandle NoHitTimer;

	UPROPERTY(EditAnywhere, Category = "Effect")
	float DestroyTime = 1.f;

	UPROPERTY(EditAnywhere, Category = "Effect")
	float ParticleOffTime = 0.5f;

	FTimerHandle CheckEnemyTimer;
	float CheckEnemyTime = 0.1f;
	
};
