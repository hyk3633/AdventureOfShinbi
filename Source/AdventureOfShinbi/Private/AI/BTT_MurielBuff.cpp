

#include "AI/BTT_MurielBuff.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MurielBuff::UBTT_MurielBuff()
{
    bNotifyTick = true;

    NodeName = TEXT("Buff");

    IsAttacking = false;
}

EBTNodeResult::Type UBTT_MurielBuff::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyMuriel* Muriel = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Muriel)
        return EBTNodeResult::Failed;

    Muriel->ProvideBuff();
    IsAttacking = true;

    Muriel->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_MurielBuff::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleBuff"), false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
