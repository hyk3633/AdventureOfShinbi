// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_ProvideBuff.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyMuriel/EnemyMuriel.h"

UBTT_ProvideBuff::UBTT_ProvideBuff()
{

}

EBTNodeResult::Type UBTT_ProvideBuff::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = true;

	AEnemyMuriel* EM = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
	if (EM == nullptr)
		return EBTNodeResult::Failed;

	bCasting = true;
	EM->ProvideBuff();

	EM->OnSkillEnd.AddLambda([this]() -> void {
		bCasting = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTT_ProvideBuff::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bCasting)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_ProvideBuff::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Aborted;
}
