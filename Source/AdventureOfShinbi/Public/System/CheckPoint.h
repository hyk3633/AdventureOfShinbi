
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CheckPoint.generated.h"

/**
* 체크 포인트 클래스
*/

class UBoxComponent;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API ACheckPoint : public APlayerStart
{
	GENERATED_BODY()
	
public:	

	ACheckPoint(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, Category = "Check Point")
	TSubclassOf<UUserWidget> CheckPointSignClass;

	UPROPERTY(EditAnywhere, Category = "Check Point")
	USoundCue* CheckSound;

	UPROPERTY()
	UUserWidget* CheckPointSign;
};
