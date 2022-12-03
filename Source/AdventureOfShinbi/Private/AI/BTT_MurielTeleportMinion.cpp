

#include "AI/BTT_MurielTeleportMinion.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MurielTeleportMinion::UBTT_MurielTeleportMinion()
{
    bNotifyTick = true;

    NodeName = TEXT("Teleport Minion");

    IsAttacking = false;
}

EBTNodeResult::Type UBTT_MurielTeleportMinion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    AEnemyMuriel* Muriel = Cast<AEnemyMuriel>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Muriel)
        return EBTNodeResult::Failed;

    Muriel->FindTeleportPosition();
    IsAttacking = true;

    Muriel->OnAttackEnd.AddLambda([this]() -> void {
        IsAttacking = false;
    });

    return EBTNodeResult::InProgress;
}

void UBTT_MurielTeleportMinion::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    if (!IsAttacking)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleTeleportMinion"), false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
