
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckSkillShotCondition.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTS_CheckSkillShotCondition : public UBTService
{
	GENERATED_BODY()
	
public:

	UBTS_CheckSkillShotCondition();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
