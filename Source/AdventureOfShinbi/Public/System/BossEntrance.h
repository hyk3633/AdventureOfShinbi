
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEntrance.generated.h"

class UBoxComponent;
class AEnemyBoss;
class AAOSCharacter;

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

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	AEnemyBoss* Boss;

	AAOSCharacter* Player;

	bool bBind = false;
};
