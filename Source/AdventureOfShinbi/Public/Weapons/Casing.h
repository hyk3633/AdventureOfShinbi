
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

/**
* 무기 메쉬에서 스폰되는 탄피 클래스
*/

class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	

	ACasing();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	UPROPERTY()
	USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CasingMesh;

	/** 탄피가 튀어나가는 강도 */
	UPROPERTY(EditAnywhere, Category = "Casing")
	float EjectImpulse = 20.f;

	UPROPERTY(EditAnywhere, Category = "Casing")
	USoundCue* CasingSound;

};
