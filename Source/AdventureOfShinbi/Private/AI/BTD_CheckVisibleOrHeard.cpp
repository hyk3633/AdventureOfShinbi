// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTD_CheckVisibleOrHeard.h"
#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTD_CheckVisibleOrHeard::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Visible = OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("TargetIsVisible"));
	bool Heard = OwnerComp.GetBlackboardComponent()->GetValueAsBool(FName("TargetIsHeard"));

	return Visible || Heard;
}