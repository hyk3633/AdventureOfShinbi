

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

    AEnemyMuriel* ControllingEnemy = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (ControllingEnemy == nullptr) return;

    if (ControllingEnemy->GetSkillShotCoolTimeEnd() == false)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSkillShot"), false);
        return;
    }

    AAOSCharacter* AC = Cast<AAOSCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName("Target")));
    if (AC)
    {
        if (AC->GetCombatComp()->GetHealthPercentage() <= 0.7f)
        {
            const float Percentage = FMath::RandRange(0.f, 1.0f);
            if (Percentage <= 0.2f)
            {
                OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSkillShot"), true);
            }
            else
            {
                OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSkillShot"), false);
            }
        }
    }
}
