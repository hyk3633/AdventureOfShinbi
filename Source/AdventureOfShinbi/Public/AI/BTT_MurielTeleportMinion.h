
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MurielTeleportMinion.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UBTT_MurielTeleportMinion : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

    UBTT_MurielTeleportMinion();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

    bool IsAttacking = false;
};
