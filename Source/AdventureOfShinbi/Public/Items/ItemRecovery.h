
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/ItemType.h"
#include "ItemRecovery.generated.h"

/**
 * 
 */

UCLASS()
class ADVENTUREOFSHINBI_API AItemRecovery : public AItem
{
	GENERATED_BODY()
	
public:

	AItemRecovery();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	ERecoveryType RecoveryType;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 500.0))
	float RecoveryQuatity = 50.f;

public:

	UStaticMeshComponent* GetStaticMesh() const;
	ERecoveryType GetRecoveryType() const;

};
