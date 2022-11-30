
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/ItemType.h"
#include "ItemRecovery.generated.h"

/**
 * 
 */

class UParticleSystem;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AItemRecovery : public AItem
{
	GENERATED_BODY()
	
public:

	AItemRecovery();

	virtual void PlayGainEffect() override;

	void PlayUsingEffect(FVector Location);

	void HandleItemAfterGain();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	ERecoveryType RecoveryType;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* FieldParticle;

	UParticleSystemComponent* FieldParticleComp;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* GainParticle;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* UsingParticle;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	USoundCue* GainSound;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	USoundCue* UsingSound;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 500.0))
	float RecoveryQuatity = 50.f;

	int8 QuickSlotIndex = -1;

public:

	UStaticMeshComponent* GetStaticMesh() const;
	ERecoveryType GetRecoveryType() const;
	void SetQuickSlotIndex(int8 Index);
	int8 GetQuickSlotIndex() const;
};
