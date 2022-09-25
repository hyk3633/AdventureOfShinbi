// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_RangedAttack.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyRanged.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RangedAttack::UBTT_RangedAttack()
{

}

EBTNodeResult::Type UBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    bNotifyTick = true;
    IsAttacking = false;

    ER = Cast<AEnemyRanged>(OwnerComp.GetAIOwner()->GetPawn());
    if (ER == nullptr)
        return EBTNodeResult::Failed;

    ER->RangedAttack();
    IsAttacking = true;

    ER->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_RangedAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}