
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEntrance.generated.h"

/**
* 보스 전투를 발생 시키는 클래스
*/

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

	/** 플레이어 사망 시 환경을 전투 전으로 설정 */
	UFUNCTION()
	void ResetEntrance();

	/** 보스전 시작 */
	void StartBossStage();

	/** 컷씬 재생 후 자동으로 호출되는 함수 */
	UFUNCTION()
	void OnLevelSequencePlayed();

	/** 컷씬 종료 후 자동으로 호출되는 함수 */
	UFUNCTION()
	void OnLevelSequenceFinished();

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BlockingVolume;

	AEnemyBoss* Boss;

	AAOSCharacter* Player;

	/** 보스 패배 델리게이트에 함수 바인딩 여부 */
	bool bBind = false;

	/** 레벨 시퀀스 플레이어 */
	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer;

	/** 컷씬 */
	UPROPERTY(EditAnywhere, Category = "Level Sequence")
	ULevelSequence* SequenceAsset;

	/** 보스 액터 */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* BossActor;

	/** 컷씬의 파티클 */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceParticle;

	/** 컷씬의 보스 액터 */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceSkeletalMesh;

	/** 컷씬 재생 여부 */
	bool bSequencePlayed = false;

};
