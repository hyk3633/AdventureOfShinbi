
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_SelectBossAttack.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTS_SelectBossAttack : public UBTService
{
	GENERATED_BODY()
	
public:

	UBTS_SelectBossAttack();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
