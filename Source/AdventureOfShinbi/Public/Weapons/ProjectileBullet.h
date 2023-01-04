// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 유도 기능이 없는 일반적인 투사체 클래스
 */

class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	
public:
	
	AProjectileBullet();

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void PlayHitEffect(const FHitResult& Hit, AActor* OtherActor);

	/** 투사체의 수명이 끝난 후 이펙트 처리 */
	void LifeOver();

	void DestroyProjectile();

protected:

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BodyParticleComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LifeSpan = 7.f;

private:

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* BulletParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* TargetHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* WorldHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* NoHitParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	USoundCue* HitSound;

	FTimerHandle LifeTimer;

	FTimerHandle DestroyTimer;
};
