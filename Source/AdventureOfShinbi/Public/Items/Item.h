
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

UCLASS()
class ADVENTUREOFSHINBI_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

	void DeactivateItemMovement();

	virtual void PlayGainEffect();

	virtual void HandleItemAfterGain();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void UpAndDownMovement(float DeltaTime);

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPickupWidgetInfo();

protected:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* Widget;

	UPROPERTY(VisibleAnywhere)
	EItemType InfoItemType;

	bool bIsWeapon = false;

	bool bOnSpin = true;

	bool bOnUpAndDown = true;

	float MovementSpeed = 30.f;

private:

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	FString ItemName;

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	EItemRank InfoItemRank;

	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	UTexture2D* ItemIcon;

	UInventorySlot* InventorySlot;

	UItemInventorySlot* ItemInventorySlot;

	float RunningTime = 0.f;

public:

	bool GetIsWeapon() const;
	USkeletalMeshComponent* GetItemMesh() const;
	UWidgetComponent* GetWidget() const;
	EItemType GetItemType() const;
	UTexture2D* GetItemIcon() const;
	void SetInventorySlot(UInventorySlot* Slot);
	void SetItemInventorySlot(UItemInventorySlot* Slot);
	UInventorySlot* GetInventorySlot() const;
	UItemInventorySlot* GetItemInventorySlot() const;

};
