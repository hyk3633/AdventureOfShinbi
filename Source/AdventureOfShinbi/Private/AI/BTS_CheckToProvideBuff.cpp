

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

    AEnemyMuriel* Muriel = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Muriel)
        return;

    if (Muriel->GetIsCasting())
        return;

    TArray<AActor*> Allies;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, AEnemyCharacter::StaticClass(), Muriel->GetFriendlyTag(), Allies);

    int8 LowHealthCount = 0;
    int8 AlliesNearbyCount = 0;
    for (AActor* const& TaggedActor : Allies)
    {
        AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
        if (Friendly)
        {
            if (Muriel->GetDistanceTo(Friendly) <= 1000.f)
            {
                AlliesNearbyCount++;
                if (Friendly->GetHealthPercentage() <= 0.6f)
                {
                    LowHealthCount++;
                }
            }
        }
    }

    if (AlliesNearbyCount < 2 && Muriel->GetSummonCoolTimeEnd())
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("LackOfAllies"), true);
    }
    
    if (LowHealthCount >= 2 && Muriel->GetBuffCoolTimeEnd())
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("ProvideBuff"), true);
    }
    
}
