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

void ABossAIController::SetBlackboardKey()
{
	if (PossessedCharacter)
	{
		BBComp->SetValueAsObject(FName("Target"), PossessedCharacter->GetTarget());

		BBComp->SetValueAsBool(FName("IsAttacking"), PossessedCharacter->GetIsAttacking());
		BBComp->SetValueAsBool(FName("Strafing"), PossessedCharacter->GetStrafing());

		BBComp->SetValueAsBool(FName("KnockUp"), PossessedCharacter->GetAiInfo().bIsKnockUp);
		BBComp->SetValueAsBool(FName("Stunned"), PossessedCharacter->GetAiInfo().bStunned);
		BBComp->SetValueAsBool(FName("IsEnemyDead"), PossessedCharacter->GetIsDead());
		BBComp->SetValueAsBool(FName("TargetInAttackRange"), PossessedCharacter->GetAiInfo().bTargetInAttackRange);
		BBComp->SetValueAsBool(FName("KeyIsPlayerDead"), PossessedCharacter->GetAiInfo().bIsPlayerDead);

		BBComp->SetValueAsBool(FName("AbleEvade"), PossessedCharacter->GetAbleEvade());
		BBComp->SetValueAsBool(FName("AbleBackAttack"), PossessedCharacter->GetAbleBackAttack());
		BBComp->SetValueAsBool(FName("AbleFreezing"), PossessedCharacter->GetAbleFreezing());
		BBComp->SetValueAsBool(FName("AbleIceWall"), PossessedCharacter->GetAbleIceWall());
	}
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABossAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetBlackboardKey();
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
