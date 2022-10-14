// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SkillShot.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SkillShot::UBTT_SkillShot()
{

}

EBTNodeResult::Type UBTT_SkillShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    bNotifyTick = true;
    IsAttacking = false;

    AEnemyMuriel* EM = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (EM == nullptr)
        return EBTNodeResult::Failed;

    IsAttacking = true;
    EM->FireSkillShot();

    EM->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_SkillShot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTT_SkillShot::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::AbortTask(OwnerComp, NodeMemory);

    return EBTNodeResult::Aborted;
}