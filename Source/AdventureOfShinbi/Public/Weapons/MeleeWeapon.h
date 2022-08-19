// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */

class UBoxComponent;

UCLASS()
class ADVENTUREOFSHINBI_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	AMeleeWeapon();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

};
