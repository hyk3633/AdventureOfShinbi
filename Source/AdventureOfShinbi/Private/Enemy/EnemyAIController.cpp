

#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
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

const FName AEnemyAIController::KeyTarget(TEXT("Target"));

const FName AEnemyAIController::KeyWaitingPosition(TEXT("WaitingPosition"));
const FName AEnemyAIController::KeyMoveToPoint(TEXT("MoveToPoint"));
const FName AEnemyAIController::KeyDetectedLocation(TEXT("DetectedLocation"));

const FName AEnemyAIController::KeyTargetIsVisible(TEXT("TargetIsVisible"));
const FName AEnemyAIController::KeySightStimulusExpired(TEXT("SightStimulusExpired"));
const FName AEnemyAIController::KeyTargetIsHeard(TEXT("TargetIsHeard"));
const FName AEnemyAIController::KeyTargetHitsMe(TEXT("TargetHitsMe"));
const FName AEnemyAIController::KeyTargetInAttackRange(TEXT("TargetInAttackRange"));
const FName AEnemyAIController::KeyStunned(TEXT("Stunned"));
const FName AEnemyAIController::KeyStiffed(TEXT("Stiffed"));
const FName AEnemyAIController::KeyKnockUp(TEXT("KnockUp"));
const FName AEnemyAIController::KeyIsAttacking(TEXT("IsAttacking"));
const FName AEnemyAIController::KeyIsPlayerDead(TEXT("IsPlayerDead"));
const FName AEnemyAIController::KeyIsEnemyDead(TEXT("IsEnemyDead"));

AEnemyAIController::AEnemyAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(5.f);

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->SetMaxAge(10.f);

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->ConfigureSense(*HearingConfig);

	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnDetected);
}

void AEnemyAIController::OnDetected(AActor* Actor, FAIStimulus Stimulus)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(Actor);

	if (Cha)
	{
		if (!IsPlayerDeathDelegateBined)
		{
			Cha->GetCombatComp()->PlayerDeathDelegate.AddLambda([this]() -> void {
				BBComp->SetValueAsBool(KeyIsPlayerDead, true);
			});
		}

		if (Stimulus.Type.Name == FName("Default__AISense_Sight"))
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				BBComp->SetValueAsBool(KeyTargetIsVisible, true);
				BBComp->SetValueAsBool(KeySightStimulusExpired, false);
				BBComp->SetValueAsObject(KeyTarget, Cha);
				if (PossessedCharacter)
				{
					PossessedCharacter->SetEnemyState(EEnemyState::EES_Chase);
				}
				GetWorldTimerManager().ClearTimer(SightStimulusExpireTimer);
			}
			else
			{
				BBComp->SetValueAsBool(KeyTargetIsVisible, false);
				GetWorldTimerManager().SetTimer(SightStimulusExpireTimer, this, &AEnemyAIController::SightStimulusExpire, SightStimulusExpireTime);
			}
		}
		else if (Stimulus.Type.Name == FName("Default__AISense_Hearing"))
		{
			if (!Stimulus.IsExpired())
			{
				BBComp->SetValueAsBool(KeyTargetIsHeard, true);
				BBComp->SetValueAsVector(KeyDetectedLocation, Cha->GetActorLocation());
				if (PossessedCharacter)
				{
					if (PossessedCharacter->GetEnemyState() == EEnemyState::EES_Patrol)
					{
						PossessedCharacter->SetEnemyState(EEnemyState::EES_Detected);
					}
				}
			}
			else
			{
				BBComp->SetValueAsBool(KeyTargetIsHeard, false);
			}
		}

		CheckNothingStimulus();
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedCharacter = Cast<AEnemyCharacter>(InPawn);

	if (UseBlackboard(BBAsset, BBComp) && PossessedCharacter)
	{
		// TODO : 기본값 세팅
		BBComp->SetValueAsObject(KeyTarget, nullptr);
		BBComp->SetValueAsBool(KeySightStimulusExpired, true);
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

void AEnemyAIController::SightStimulusExpire()
{
	BBComp->SetValueAsBool(KeySightStimulusExpired, true);
	BBComp->SetValueAsObject(KeyTarget, nullptr);

	CheckNothingStimulus();
}

void AEnemyAIController::CheckNothingStimulus()
{
	if (!BBComp->GetValueAsBool(KeyTargetIsVisible) &&
		BBComp->GetValueAsBool(KeySightStimulusExpired) &&
		!BBComp->GetValueAsBool(KeyTargetIsHeard) &&
		!BBComp->GetValueAsBool(KeyTargetHitsMe))
	{
		if (PossessedCharacter)
		{
			if (PossessedCharacter->GetEnemyState() == EEnemyState::EES_Chase ||
				PossessedCharacter->GetEnemyState() == EEnemyState::EES_Detected )
			{
				PossessedCharacter->SetEnemyState(EEnemyState::EES_Comeback);
			}
		}
	}
}

void AEnemyAIController::SetStateToPatrol()
{
	if (PossessedCharacter->GetEnemyState() == EEnemyState::EES_Comeback)
	{
		PossessedCharacter->SetEnemyState(EEnemyState::EES_Patrol);
	}
}

UBlackboardComponent* AEnemyAIController::GetBlackBoard() const
{
	return BBComp;
}
