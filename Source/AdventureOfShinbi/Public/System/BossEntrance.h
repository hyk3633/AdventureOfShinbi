
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEntrance.generated.h"

/**
* ���� ������ �߻� ��Ű�� Ŭ����
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

	/** �÷��̾� ��� �� ȯ���� ���� ������ ���� */
	UFUNCTION()
	void ResetEntrance();

	/** ������ ���� */
	void StartBossStage();

	/** �ƾ� ��� �� �ڵ����� ȣ��Ǵ� �Լ� */
	UFUNCTION()
	void OnLevelSequencePlayed();

	/** �ƾ� ���� �� �ڵ����� ȣ��Ǵ� �Լ� */
	UFUNCTION()
	void OnLevelSequenceFinished();

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BlockingVolume;

	AEnemyBoss* Boss;

	AAOSCharacter* Player;

	/** ���� �й� ��������Ʈ�� �Լ� ���ε� ���� */
	bool bBind = false;

	/** ���� ������ �÷��̾� */
	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer;

	/** �ƾ� */
	UPROPERTY(EditAnywhere, Category = "Level Sequence")
	ULevelSequence* SequenceAsset;

	/** ���� ���� */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* BossActor;

	/** �ƾ��� ��ƼŬ */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceParticle;

	/** �ƾ��� ���� ���� */
	UPROPERTY(EditInstanceOnly, Category = "Level Sequence")
	AActor* SequenceSkeletalMesh;

	/** �ƾ� ��� ���� */
	bool bSequencePlayed = false;

};
