
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEntrance.generated.h"

class UBoxComponent;
class AEnemyBoss;
class AAOSCharacter;
class ULevelSequencePlayer;
class ULevelSequence;
class UParticleSystemComponent;

UCLASS()
class ADVENTUREOFSHINBI_API ABossEntrance : public AActor
{
	GENERATED_BODY()
	
public:	

	ABossEntrance();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ResetEntrance();

	void StartBossStage();

	UFUNCTION()
	void OnLevelSequencePlayed();

	UFUNCTION()
	void OnLevelSequenceFinished();

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BlockingVolume;

	AEnemyBoss* Boss;

	AAOSCharacter* Player;

	bool bBind = false;

	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer;

	UPROPERTY(EditAnywhere, Category = "Level Sequence")
	ULevelSequence* SequenceAsset;

	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* BossActor;

	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceParticle;

	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceSkeletalMesh;

	bool bSequencePlayed = false;

};
