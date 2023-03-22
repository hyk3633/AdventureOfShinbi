
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPool.generated.h"

class AProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UObjectPool : public UActorComponent
{
	GENERATED_BODY()

public:	

	UObjectPool();

	AProjectile* GetPooledActor();

	void StartPooling();

	void DestroyPool();

	FORCEINLINE void SetOwner(AActor* NewOwner) { ThisOwner = NewOwner; };

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void AddDeactivatedNum();

public:	
	
	UPROPERTY(EditAnywhere, Category = "Object Pooler")
	TSubclassOf<AProjectile> ProjectileSubclass;
	
	UPROPERTY(EditAnywhere, Category = "Object Pooler")
	int PoolSize = 30;

	UPROPERTY(VisibleAnywhere, Category = "Object Pooler")
	TArray<AProjectile*> Pool;

	AActor* ThisOwner;

	UPROPERTY(VisibleAnywhere)
	int8 ActivatedNum;

	UPROPERTY(VisibleAnywhere)
	int8 DeactivatedNum;

};
