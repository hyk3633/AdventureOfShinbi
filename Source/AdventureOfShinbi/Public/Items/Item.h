
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ItemRank.h"
#include "Types/ItemType.h"
#include "Item.generated.h"

class USphereComponent;
class UWidgetComponent;
class UPickupWidget;
class UTexture2D;
class UInventorySlot;
class UItemInventorySlot;

/**
* ������ �⺻ Ŭ����
*/

UCLASS()
class ADVENTUREOFSHINBI_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

	/** ������ ������ ��Ȱ��ȭ */
	void DeactivateItemMovement();

	/** ȹ�� ����Ʈ ��� ���� �Լ� */
	virtual void PlayGainEffect();

	/** ������ ȹ�� �� ������, �ݸ��� ���� ó�� */
	virtual void HandleItemAfterGain();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** �������� ���Ʒ��� �����̴� �Լ� */
	void UpAndDownMovement(float DeltaTime);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPickupWidgetInfo();

protected:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ItemMesh;

	/** ������ ������ ���� ���� �ݸ��� ����� */
	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	/** ������ ���� ���� */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* Widget;

	/** ������ ���� */
	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	EItemType InfoItemType;

	/** ȸ�� �� ���� */
	bool bOnSpin = true;

	/** ���Ʒ��� �����̴� �� ���� */
	bool bOnUpAndDown = true;

	/** ������ �ӵ� */
	float MovementSpeed = 30.f;

private:

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	FString ItemName;

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	EItemRank InfoItemRank;

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	UTexture2D* ItemIcon;

	UPROPERTY()
	UInventorySlot* InventorySlot;

	UPROPERTY()
	UItemInventorySlot* ItemInventorySlot;

	float RunningTime = 0.f;

public:

	USkeletalMeshComponent* GetItemMesh() const;
	UWidgetComponent* GetWidget() const;
	EItemType GetItemType() const;
	UTexture2D* GetItemIcon() const;
	void SetInventorySlot(UInventorySlot* Slot);
	void SetItemInventorySlot(UItemInventorySlot* Slot);
	UInventorySlot* GetInventorySlot() const;
	UItemInventorySlot* GetItemInventorySlot() const;

};
