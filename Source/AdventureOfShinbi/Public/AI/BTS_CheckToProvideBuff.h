// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckToProvideBuff.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTS_CheckToProvideBuff : public UBTService
{
	GENERATED_BODY()
	
public:

	UBTS_CheckToProvideBuff();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
