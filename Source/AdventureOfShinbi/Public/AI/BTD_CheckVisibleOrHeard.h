// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTD_CheckVisibleOrHeard.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTD_CheckVisibleOrHeard : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

public:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
