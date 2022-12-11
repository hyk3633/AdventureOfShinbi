
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransitionVolume.generated.h"

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

	UFUNCTION()
	void ReBindFunction();

	UFUNCTION()
	void LevelTransition();

	void ShowLevelTrasitionSign();

	void OpenNextLevel();

private:

	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* MarkParticle;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	FString LevelName;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	TSubclassOf<UUserWidget> LevelTrasnsitionSignClass;

	UUserWidget* LevelTrasnsitionSign;

	UPROPERTY(EditAnywhere, Category = "Level Transition")
	TSubclassOf<UUserWidget> LoadingScreen;

	FTimerHandle TransitionTimer;
};
