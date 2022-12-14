// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SiegeModeAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTT_SiegeModeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

    UBTT_SiegeModeAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

    bool IsAttacking = false;
};
