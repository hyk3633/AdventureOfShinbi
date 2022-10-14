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

	void Weapon2LineTrace();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponTrace2();

private:

	bool bActivateWeaponTrace2 = false;

};
