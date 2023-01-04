
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/AmmoType.h"
#include "ItemAmmo.generated.h"

/**
 * ≈∫æ‡ æ∆¿Ã≈€ ≈¨∑°Ω∫
 */

class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AItemAmmo : public AItem
{
	GENERATED_BODY()

public:

	AItemAmmo();

	virtual void PlayGainEffect() override;

	virtual void HandleItemAfterGain() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	/** ≈∫æ‡ ¿Ø«¸ */
	UPROPERTY(EditAnywhere)
	EAmmoType AmmoType;

	/** ≈∫æ‡∑Æ */
	UPROPERTY(EditAnywhere)
	int32 AmmoQuantity = 10;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	USoundCue* GainSound;

public:

	UStaticMeshComponent* GetStaticMesh() const;
	EAmmoType GetAmmoType() const;
	int32 GetAmmoQuantity() const;
};
