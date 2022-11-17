// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

/**
 * 
 */

class AEnemyBoss;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UBlackboardComponent;
class UBlackboardData;

UCLASS()
class ADVENTUREOFSHINBI_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:

	ABossAIController();

	void UpdateAiInfo();

	void SetTarget(AActor* Target);

	void SetRangedAttackCoolTimeEnd(bool Key);

	void SetAbleEvadeSkill(bool Key);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

private:

	AEnemyBoss* PossessedCharacter;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BBAsset;

	UBehaviorTreeComponent* BTComp;

	UBlackboardComponent* BBComp;
};
