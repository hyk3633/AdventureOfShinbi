

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

    if (Muriel->GetIsAttacking())
        return;

    TArray<AActor*> Allies;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, AEnemyCharacter::StaticClass(), Muriel->GetFriendlyTag(), Allies);

    int8 AlliesNearbyCount = 0;
    int8 LowHealthCount = 0;
    for (AActor* const& TaggedActor : Allies)
    {
        AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
        if (Friendly)
        {
            if (Muriel->GetDistanceTo(Friendly) <= 1250.f)
            {
                AlliesNearbyCount++;
                if (Friendly->GetHealthPercentage() <= 0.6f)
                {
                    LowHealthCount++;
                }
            }
        }
    }
    
    if (Muriel->GetSummonCoolTimeEnd() && AlliesNearbyCount <= 1)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSummonMinion"), true);
    }
    else if (Muriel->GetBuffCoolTimeEnd() && LowHealthCount >= 2)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleBuff"), true);
    }
    else if (Muriel->GetTeleportMinionCoolTimeEnd() && AlliesNearbyCount >= 1)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleTeleportMinion"), true);
    }
}
