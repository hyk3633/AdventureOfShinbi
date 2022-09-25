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

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileHoming : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileHoming();

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void DestroyProjectile();

	void ParticleOff();

private:

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystemComponent* BodyParticleComponent;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* FlyingSound;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* ImpactSound;

	FTimerHandle DestroyTimer;
	FTimerHandle ParticleOffTimer;

	UPROPERTY(EditAnywhere, Category = "Effect")
	float DestroyTime = 1.f;

	UPROPERTY(EditAnywhere, Category = "Effect")
	float ParticleOffTime = 0.5f;
	
};
