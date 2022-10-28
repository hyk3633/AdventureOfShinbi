// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
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

	void PlayNoHitParticle();

protected:

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* BodyParticleComponent;

	UPROPERTY(EditAnywhere)
	float LifeSpan = 7.f;

private:

	// 투사체 몸체 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* BulletParticle;

	// 타겟 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* TargetHitParticle;

	// 타겟이 아닌 물체 적중 시 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* WorldHitParticle;

	// 무엇도 적중하지 않을 경우의 파티클
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* NoHitParticle;

	// 적중 사운드
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* HitSound;

	FTimerHandle NoHitTimer;
};
