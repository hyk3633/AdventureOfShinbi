
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

/**
* 레벨 트랜지션 볼륨 클래스
*/

class UBoxComponent;
class UParticleSystemComponent;
UCLASS()
class ADVENTUREOFSHINBI_API ALevelTransitionVolume : public AActor
{
	GENERATED_BODY()
	
public:	

	ALevelTransitionVolume();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** 플레이어 리스폰 시 함수 재 할당 */
	UFUNCTION()
	void ReBindFunction();

	/** 데이터 저장, 로딩 스크린 출력 후 다음 레벨로 이동 */
	UFUNCTION()
	void LevelTransition();

	/** 사인 위젯 출력 */
	void ShowLevelTrasitionSign();

	/** 다음 레벨로 이동 */
	void OpenNextLevel();

private:

	
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* MarkParticle;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	/** 이동 할 레벨의 이름 */
	UPROPERTY(EditAnywhere, Category = "Level Transition")
	FString LevelName;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	TSubclassOf<UUserWidget> LevelTrasnsitionSignClass;

	UPROPERTY()
	UUserWidget* LevelTrasnsitionSign;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	TSubclassOf<UUserWidget> LoadingScreen;

	FTimerHandle TransitionTimer;
};
