

#include "AI/BTT_MurielSummonMinion.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MurielSummonMinion::UBTT_MurielSummonMinion()
{
    bNotifyTick = true;

    NodeName = TEXT("Summon Minion");

    IsAttacking = false;
}

EBTNodeResult::Type UBTT_MurielSummonMinion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyMuriel* Muriel = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Muriel)
        return EBTNodeResult::Failed;

    Muriel->SummonMinion();
    IsAttacking = true;

    Muriel->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_MurielSummonMinion::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleSummonMinion"), false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
