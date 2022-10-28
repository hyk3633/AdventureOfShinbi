

#include "AI/BTS_CheckToProvideBuff.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Player/AOSCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UBTS_CheckToProvideBuff::UBTS_CheckToProvideBuff()
{
	Interval = 1.f;
}

void UBTS_CheckToProvideBuff::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AEnemyMuriel* ControllingEnemy = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == ControllingEnemy)
        return;

    TArray<AActor*> Allies;
    UGameplayStatics::GetAllActorsWithTag(this, ControllingEnemy->GetFriendlyTag(), Allies);

    int8 LowHealthCount = 0;
    int8 AlliesNearbyCount = 0;
    for (AActor* const& TaggedActor : Allies)
    {
        AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
        if (Friendly)
        {
            if (ControllingEnemy->GetDistanceTo(Friendly) <= 1000.f)
            {
                AlliesNearbyCount++;
                if (Friendly->GetHealthPercentage() <= 0.6f)
                {
                    LowHealthCount++;
                }
            }
        }
    }

    if (AlliesNearbyCount < 2 && ControllingEnemy->GetSummonCoolTimeEnd())
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("LackOfAllies"), true);
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("LackOfAllies"), false);
    }
    
    if (LowHealthCount >= 2 && ControllingEnemy->GetBuffCoolTimeEnd())
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("ProvideBuff"), true);
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("ProvideBuff"), false);
    }
    
}
