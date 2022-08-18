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

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	
public:

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	float LifeSpan = 3.f;
};
