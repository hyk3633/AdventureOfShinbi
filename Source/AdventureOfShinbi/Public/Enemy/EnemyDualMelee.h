// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyDualMelee.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API AEnemyDualMelee : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AEnemyDualMelee();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable)
	void ActivateDamageCollision2();

	UFUNCTION(BlueprintCallable)
	void DeactivateDamageCollision2();
	
private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision2;
};
