// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SiegeModeAttack.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyRangedSiege.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SiegeModeAttack::UBTT_SiegeModeAttack()
{
	bNotifyTick = true;

	IsAttacking = false;
}

EBTNodeResult::Type UBTT_SiegeModeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyRangedSiege* ES = Cast<AEnemyRangedSiege>(OwnerComp.GetAIOwner()->GetPawn());
    if (ES == nullptr)
        return EBTNodeResult::Failed;

    ES->SiegeModeAttack();
    IsAttacking = true;

    ES->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_SiegeModeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
