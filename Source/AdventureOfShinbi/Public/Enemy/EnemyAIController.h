
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */

class AEnemyCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
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

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDetected(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	void SightStimulusExpire();

	static const FName KeyTarget;
	static const FName KeyAimedTarget;

	static const FName KeyWaitingPosition;
	static const FName KeyDetectedLocation;

	static const FName KeyTargetIsVisible;
	static const FName KeySightStimulusExpired;
	static const FName KeyTargetIsHeard;
	static const FName KeyTargetHitsMe;
	static const FName KeyTargetInAttackRange;
	static const FName KeyTargetInChaseRange;
	static const FName KeyStunned;
	static const FName KeyStiffed;
	static const FName KeyKnockUp;
	static const FName KeyIsAttacking;
	static const FName KeyIsPlayerDead;
	static const FName KeyIsEnemyDead;

private:

	AEnemyCharacter* PossessedCharacter;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BBAsset;

	UBehaviorTreeComponent* BTComp;

	UBlackboardComponent* BBComp;

	FTimerHandle SightStimulusExpireTimer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float SightStimulusExpireTime = 10.f;

	bool IsPlayerDeathDelegateBined = false;

public:

	UBlackboardComponent* GetBlackBoard() const;

	void CheckNothingStimulus();

	UFUNCTION(BlueprintCallable)
	void SetStateToPatrol();
};
