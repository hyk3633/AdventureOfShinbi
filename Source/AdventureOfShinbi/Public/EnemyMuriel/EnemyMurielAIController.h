// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyMurielAIController.generated.h"

/**
 * 
 */

class AEnemyMuriel;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyMurielAIController : public AEnemyAIController
{
	GENERATED_BODY()
	
public:

	AEnemyMurielAIController();

protected:

	virtual void Tick(float DeltaTime) override;

private:

	AEnemyMuriel* Muriel;

};
