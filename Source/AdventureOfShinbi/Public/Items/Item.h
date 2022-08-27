// Fill out your copyright notice in the Description page of Project Settings.

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

UCLASS()
class ADVENTUREOFSHINBI_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPickupWidgetInfo();

	UPROPERTY(VisibleAnywhere)
	EItemType InfoItemType;

private:

	UPROPERTY(EditAnywhere)
	FString ItemName;

	UPROPERTY(EditAnywhere)
	EItemRank InfoItemRank;

	UPROPERTY(EditAnywhere)
	UTexture2D* ItemIcon;

	UInventorySlot* InventorySlot;

protected:

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* Widget;

	bool bIsWeapon = false;

public:

	bool GetIsWeapon() const;
	USkeletalMeshComponent* GetItemMesh() const;
	UWidgetComponent* GetWidget() const;
	EItemType GetItemType() const;
	UTexture2D* GetItemIcon() const;
	void SetInventorySlot(UInventorySlot* Slot);
	UInventorySlot* GetInventorySlot() const;

};
