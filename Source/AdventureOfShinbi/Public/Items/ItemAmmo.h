// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/AmmoType.h"
#include "ItemAmmo.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API AItemAmmo : public AItem
{
	GENERATED_BODY()

public:

	AItemAmmo();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere)
	int32 AmmoQuantity = 10;

public:

	UStaticMeshComponent* GetStaticMesh() const;
	EAmmoType GetAmmoType() const;
	int32 GetAmmoQuantity() const;
};