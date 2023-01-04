// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_Strafing.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Strafing::UBTT_Strafing()
{
	bNotifyTick = true;

	NodeName = TEXT("Strafing");
}

EBTNodeResult::Type UBTT_Strafing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyStrafing* ES = Cast<AEnemyStrafing>(OwnerComp.GetAIOwner()->GetPawn());
    if (ES == nullptr)
        return EBTNodeResult::Failed;

    bStrafing = true;

    ES->StartStrafing();

    ES->OnStrafingEnd.AddLambda([this]() -> void {
        bStrafing = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_Strafing::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!bStrafing)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type UBTT_Strafing::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Aborted;
}
