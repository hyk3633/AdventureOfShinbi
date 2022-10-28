// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_Boss_BackAttack.h"
#include "EnemyBoss/BossAIController.h"
#include "EnemyBoss/EnemyBoss.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Boss_BackAttack::UBTT_Boss_BackAttack()
{
	bNotifyTick = true;

	NodeName = TEXT("Back Attack");

	IsAttacking = false;
}

EBTNodeResult::Type UBTT_Boss_BackAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyBoss* EB = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
    if (EB == nullptr)
        return EBTNodeResult::Failed;

    EB->BackAttack();
    IsAttacking = true;

    EB->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTT_Boss_BackAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::AbortTask(OwnerComp, NodeMemory);

    return EBTNodeResult::Aborted;
}

void UBTT_Boss_BackAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleBackAttack"), false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
