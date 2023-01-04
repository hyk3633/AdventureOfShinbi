

#include "AI/BTS_CheckDistanceForStrafing.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyStrafing/EnemyStrafing.h"
#include "Player/AOSCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "DrawDebugHelpers.h"

UBTS_CheckDistanceForStrafing::UBTS_CheckDistanceForStrafing()
{
	NodeName = TEXT("Check Distance for Strafing");
	Interval = 0.5f;
}

void UBTS_CheckDistanceForStrafing::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
    Super::OnSearchStart(SearchData);

}

void UBTS_CheckDistanceForStrafing::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AEnemyStrafing* ControllingEnemy = Cast<AEnemyStrafing>(OwnerComp.GetAIOwner()->GetPawn());
    if (ControllingEnemy == nullptr) 
        return;

    UWorld* World = ControllingEnemy->GetWorld();

    if (World == nullptr) 
        return;

    AActor* TargetActor = ControllingEnemy->GetAiInfo().TargetPlayer;
    if (TargetActor == nullptr)
        return;
    
    AAOSCharacter* Cha = Cast<AAOSCharacter>(TargetActor);
    if (Cha && !OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("SightStimulusExpired")))
    {
        const float Distance = ControllingEnemy->GetDistanceTo(Cha);

        if (Distance <= 900.f && ControllingEnemy->GetIsAttacking() == false)
        {
            KeepingTime += DeltaSeconds;
            if (KeepingTime >= 1.f)
            {
                OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), true);
                KeepingTime = 0.f;
            }
        }
        else
        {
            KeepingTime = 0.f;
            OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), false);
            ControllingEnemy->OnStrafingEnd.Broadcast();
        }
    }
    else
    {
        KeepingTime = 0.f;
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), false);
        ControllingEnemy->OnStrafingEnd.Broadcast();
    }
}
