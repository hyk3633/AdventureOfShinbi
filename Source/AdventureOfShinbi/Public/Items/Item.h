
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
* 아이템 기본 클래스
*/

UCLASS()
class ADVENTUREOFSHINBI_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

	/** 아이템 움직임 비활성화 */
	void DeactivateItemMovement();

	/** 획득 이펙트 재생 가상 함수 */
	virtual void PlayGainEffect();

	/** 아이템 획득 후 피직스, 콜리전 등의 처리 */
	virtual void HandleItemAfterGain();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 아이템을 위아래로 움직이는 함수 */
	void UpAndDownMovement(float DeltaTime);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPickupWidgetInfo();

protected:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ItemMesh;

	/** 아이템 정보를 띄우기 위한 콜리전 스페어 */
	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	/** 아이템 정보 위젯 */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* Widget;

	/** 아이템 유형 */
	UPROPERTY(EditAnywhere, Category = "Item | Setting")
	EItemType InfoItemType;

	/** 회전 중 인지 */
	bool bOnSpin = true;

	/** 위아래로 움직이는 중 인지 */
	bool bOnUpAndDown = true;

	/** 움직임 속도 */
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
