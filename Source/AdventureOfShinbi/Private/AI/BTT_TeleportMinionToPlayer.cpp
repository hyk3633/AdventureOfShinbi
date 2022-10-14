// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_TeleportMinionToPlayer.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyMuriel/EnemyMuriel.h"


UBTT_TeleportMinionToPlayer::UBTT_TeleportMinionToPlayer()
{

}

EBTNodeResult::Type UBTT_TeleportMinionToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	bNotifyTick = true;

	AEnemyMuriel* EM = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
	if (EM == nullptr)
		return EBTNodeResult::Failed;

	bCasting = true;
	EM->FindTeleportPosition();

	EM->OnSkillEnd.AddLambda([this]() -> void {
		bCasting = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTT_TeleportMinionToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bCasting == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTT_TeleportMinionToPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Aborted;
}
