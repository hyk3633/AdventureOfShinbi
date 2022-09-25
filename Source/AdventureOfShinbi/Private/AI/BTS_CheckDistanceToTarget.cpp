// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_CheckDistanceToTarget.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyRangedSiege.h"
#include "Player/AOSCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "DrawDebugHelpers.h"

UBTS_CheckDistanceToTarget::UBTS_CheckDistanceToTarget()
{
	NodeName = TEXT("Check Distance to Target");
	Interval = 1.f;
}

void UBTS_CheckDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AEnemyRangedSiege* ControllingEnemy = Cast<AEnemyRangedSiege>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == ControllingEnemy)
        return;

    UWorld* World = ControllingEnemy->GetWorld();
    FVector Center = ControllingEnemy->GetActorLocation();
    float DetectRadius = 6000.0f;

    if (nullptr == World)
        return;

    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingEnemy);
    bool bResult = World->OverlapMultiByChannel(OverlapResults, Center, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(DetectRadius), CollisionQueryParam);

    if (bResult)
    {
        for (FOverlapResult const& OverlapResult : OverlapResults)
        {
            AAOSCharacter* Cha = Cast<AAOSCharacter>(OverlapResult.GetActor());
            if (Cha && !OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("SightStimulusExpired")))
            {
                float Distance = ControllingEnemy->GetDistanceTo(Cha);
                if (Distance >= 2500.f && Distance <= 5000.f)
                {
                    ControllingEnemy->ConvertSiegeMode();
                }
                else
                {
                    ControllingEnemy->ReleaseSiegeMode();
                }
                return;
            }
        }
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("SiegeMode"), false);
}
