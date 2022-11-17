
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */

class AEnemyCharacter;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UBlackboardComponent;
class UBlackboardData;

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyAIController();

	void SetTarget(AActor* Target);

	void SetDetectedLocation(FVector Location);

	void UpdateAiInfo();

	void ActivateSiegeMode();
	void DeactivateSiegeMode();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

private:

	AEnemyCharacter* PossessedCharacter;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BBAsset;

	UBehaviorTreeComponent* BTComp;

	UBlackboardComponent* BBComp;

};
