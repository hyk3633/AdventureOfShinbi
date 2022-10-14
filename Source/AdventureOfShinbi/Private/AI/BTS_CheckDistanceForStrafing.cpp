

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
    if (ControllingEnemy == nullptr) return;

    UWorld* World = ControllingEnemy->GetWorld();
    FVector Center = ControllingEnemy->GetActorLocation();
    float DetectRadius = 1000.0f;

    if (World == nullptr) return;

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

                if (Distance <= 900.f)
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), true);
                }
                else
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("Strafing"), false);
                    ControllingEnemy->OnStrafingEnd.Broadcast();
                }

                return;
            }
        }
    }
}
