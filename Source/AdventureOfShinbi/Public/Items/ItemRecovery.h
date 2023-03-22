
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/ItemType.h"
#include "ItemRecovery.generated.h"

/**
 * ȸ�� ������ Ŭ����
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

	virtual void HandleItemAfterGain() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	/** ȸ�� ���� */
	UPROPERTY(EditAnywhere)
	ERecoveryType RecoveryType;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* FieldParticle;

	UPROPERTY()
	UParticleSystemComponent* FieldParticleComp;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* GainParticle;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	UParticleSystem* UsingParticle;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	USoundCue* GainSound;

	UPROPERTY(EditAnywhere, Category = "Item | Effect")
	USoundCue* UsingSound;

	/** ȸ���� */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0))
	float RecoveryQuantity = 50.f;

	/** ������ �ε���, ����� �ȵǾ� ������ -1 */
	int8 QuickSlotIndex = -1;

	bool bAutoActivateEffect = true;

public:

	UStaticMeshComponent* GetStaticMesh() const;
	ERecoveryType GetRecoveryType() const;
	void SetQuickSlotIndex(int8 Index);
	int8 GetQuickSlotIndex() const;
	float GetRecoveryQuantity() const;
};
