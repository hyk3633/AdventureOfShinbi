

#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyRangedSiege.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AEnemyAIController::AEnemyAIController()
{

}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyAIController::SetTarget(AActor* Target)
{
	BBComp->SetValueAsObject(FName("Target"), Target);
}

void AEnemyAIController::SetDetectedLocation(FVector Location)
{
	BBComp->SetValueAsVector(FName("DetectedLocation"), Location);
}

void AEnemyAIController::UpdateAiInfo()
{
	if (PossessedCharacter)
	{
		BBComp->SetValueAsBool(FName("TargetIsVisible"), PossessedCharacter->GetAiInfo().bTargetIsVisible);
		BBComp->SetValueAsBool(FName("SightStimulusExpired"), PossessedCharacter->GetAiInfo().bSightStimulusExpired);
		BBComp->SetValueAsBool(FName("TargetIsHeard"), PossessedCharacter->GetAiInfo().bTargetIsHeard);
		BBComp->SetValueAsBool(FName("IsAttacking"), PossessedCharacter->GetIsAttacking());

		BBComp->SetValueAsBool(FName("KnockUp"), PossessedCharacter->GetAiInfo().bIsKnockUp);
		BBComp->SetValueAsBool(FName("Stunned"), PossessedCharacter->GetAiInfo().bStunned);
		BBComp->SetValueAsBool(FName("Stiffed"), PossessedCharacter->GetAiInfo().bStiffed);
		BBComp->SetValueAsBool(FName("IsEnemyDead"), PossessedCharacter->GetIsDead());
		BBComp->SetValueAsBool(FName("TargetInAttackRange"), PossessedCharacter->GetAiInfo().bTargetInAttackRange);
		BBComp->SetValueAsBool(FName("TargetHitsMe"), PossessedCharacter->GetAiInfo().bTargetHitsMe);
		BBComp->SetValueAsBool(FName("IsPlayerDead"), PossessedCharacter->GetAiInfo().bIsPlayerDead);
	}
}

void AEnemyAIController::ActivateSiegeMode()
{
	BBComp->SetValueAsBool(FName("SiegeMode"), true);
}

void AEnemyAIController::DeactivateSiegeMode()
{
	BBComp->SetValueAsBool(FName("SiegeMode"), false);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<AEnemyCharacter>(InPawn);

	if (UseBlackboard(BBAsset, BBComp) && PossessedCharacter)
	{
		BBComp->SetValueAsObject(FName("Target"), nullptr);
		BBComp->SetValueAsBool(FName("SightStimulusExpired"), true);
		BBComp->SetValueAsVector(FName("WaitingPosition"), PossessedCharacter->GetPatrolPoint());
		BBComp->SetValueAsVector(FName("MoveToPoint"), PossessedCharacter->GetActorLocation());
	}

	if (BTAsset)
	{
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed behaviortree"));
		}
	}
}

void AEnemyAIController::OnUnPossess()
{
}