// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckDistanceForStrafing.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTS_CheckDistanceForStrafing : public UBTService
{
	GENERATED_BODY()

public:

	UBTS_CheckDistanceForStrafing();

protected:

	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
