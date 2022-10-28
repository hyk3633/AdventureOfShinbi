// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileBullet.h"
#include "ProjectileBossSwordAura.generated.h"

/**
 * 
 */

class AAOSCharacter;

UCLASS()
class ADVENTUREOFSHINBI_API AProjectileBossSwordAura : public AProjectileBullet
{
	GENERATED_BODY()
	
public:

	AProjectileBossSwordAura();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void SlowTimeEnd();

private:

	AAOSCharacter* Target;

	FTimerHandle SlowTimer;
	
	UPROPERTY(EditAnywhere)
	float SlowTime = 5.f;


};