// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SwapPatrolPoint.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

UBTT_SwapPatrolPoint::UBTT_SwapPatrolPoint()
{
    NodeName = TEXT("SwapPatrolPosition");
}

EBTNodeResult::Type UBTT_SwapPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    const FVector MoveToPoint = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("MoveToPoint"));
    const FVector WaitingPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(FName("WaitingPosition"));

    OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("WaitingPosition"), MoveToPoint);
    OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("MoveToPoint"), WaitingPosition);

    return EBTNodeResult::Succeeded;
}