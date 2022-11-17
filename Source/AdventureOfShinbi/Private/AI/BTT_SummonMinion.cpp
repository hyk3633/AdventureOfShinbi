// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_SummonMinion.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyMuriel/EnemyMuriel.h"

UBTT_SummonMinion::UBTT_SummonMinion()
{

}

EBTNodeResult::Type UBTT_SummonMinion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = true;

	AEnemyMuriel* EM = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
	if (EM == nullptr)
		return EBTNodeResult::Failed;

	bCasting = true;
	EM->SummonMinion();

	EM->OnSkillEnd.AddLambda([this]() -> void {
		bCasting = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTT_SummonMinion::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bCasting)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("LackOfAllies"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_SummonMinion::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Aborted;
}
