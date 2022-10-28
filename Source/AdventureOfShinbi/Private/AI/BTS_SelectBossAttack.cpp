
#include "AI/BTS_SelectBossAttack.h"
#include "Enemy/EnemyAIController.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Player/AOSCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UBTS_SelectBossAttack::UBTS_SelectBossAttack()
{

}

void UBTS_SelectBossAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyBoss* ControlledBoss = Cast<AEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControlledBoss)
		return;

	if (ControlledBoss->GetTarget() == nullptr)
		return;

	if (ControlledBoss->GetIsAttacking())
		return;

	if (ControlledBoss->GetDistanceTo(ControlledBoss->GetTarget()) <= 900.f)
	{
		if (ControlledBoss->GetBlizzardCoolTimeEnd())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleBlizzard"), true);
		}
	}
	else
	{
		if (ControlledBoss->GetDashCoolTimeEnd())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleDash"), true);
		}
		else if (ControlledBoss->GetEmitSwordAuraCoolTimeEnd())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleEmitSwordAura"), true);
		}
		else if (ControlledBoss->GetIcicleAttackCoolTimeEnd())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("AbleIcicleAttack"), true);
		}
	} 
}
