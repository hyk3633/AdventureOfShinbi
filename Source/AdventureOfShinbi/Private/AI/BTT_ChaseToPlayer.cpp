// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_ChaseToPlayer.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAIController.h"

UBTT_ChaseToPlayer::UBTT_ChaseToPlayer()
{

}

EBTNodeResult::Type UBTT_ChaseToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyCharacter* EC = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (EC)
	{
		AcceptableRadius = EC->GetAcceptableRaius();
	}

	return EBTNodeResult::InProgress;
}