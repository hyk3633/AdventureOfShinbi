// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss/BossAIController.h"
#include "EnemyBoss/EnemyBoss.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"

ABossAIController::ABossAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossAIController::UpdateAiInfo()
{
	if (PossessedCharacter)
	{
		BBComp->SetValueAsBool(FName("IsAttacking"), PossessedCharacter->GetIsAttacking());
		BBComp->SetValueAsBool(FName("Stunned"), PossessedCharacter->GetAiInfo().bStunned);
		BBComp->SetValueAsBool(FName("IsEnemyDead"), PossessedCharacter->GetIsDead());
		BBComp->SetValueAsBool(FName("TargetInAttackRange"), PossessedCharacter->GetAiInfo().bTargetInAttackRange);
		BBComp->SetValueAsBool(FName("IsPlayerDead"), PossessedCharacter->GetAiInfo().bIsPlayerDead);
	}
}

void ABossAIController::SetTarget(AActor* Target)
{
	BBComp->SetValueAsObject(FName("Target"), Target);
}

void ABossAIController::SetRangedAttackCoolTimeEnd(bool Key)
{
	if (PossessedCharacter)
	{
		BBComp->SetValueAsBool(FName("RangedAttackCoolTimeEnd"), Key);
	}
}

void ABossAIController::SetAbleEvadeSkill(bool Key)
{
	if (PossessedCharacter)
	{
		BBComp->SetValueAsBool(FName("AbleEvadeSkill"), Key);
	}
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABossAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<AEnemyBoss>(InPawn);
	if (PossessedCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Character"));
	}

	if (UseBlackboard(BBAsset, BBComp))
	{

	}

	if (BTAsset)
	{
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed behaviortree"));
		}
	}
}

void ABossAIController::OnUnPossess()
{

}
