// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RangedAttack.generated.h"

/**
 * 
 */

class AEnemyRanged;

UCLASS()
class ADVENTUREOFSHINBI_API UBTT_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

    UBTT_RangedAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

    bool IsAttacking = false;
    AEnemyRanged* ER;
};
