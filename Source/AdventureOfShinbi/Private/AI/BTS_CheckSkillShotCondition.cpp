

#include "AI/BTS_CheckSkillShotCondition.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Kismet/GameplayStatics.h"

UBTS_CheckSkillShotCondition::UBTS_CheckSkillShotCondition()
{
    Interval = 2.f;
}

void UBTS_CheckSkillShotCondition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AEnemyMuriel* Muriel = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (Muriel == nullptr)
        return;

    if (Muriel->GetIsCasting() || Muriel->GetIsAttacking())
        return;

    AAOSCharacter* AC = Cast<AAOSCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName("Target")));
    if (AC)
    {
        if (AC->GetCombatComp()->GetHealthPercentage() <= 0.7f && Muriel->GetSkillShotCoolTimeEnd())
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSkillShot"), true);
        }
    }
}
