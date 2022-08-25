// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AOSHUD.generated.h"

/**
 * 
 */

class UUserWidget;
class UAOSCharacterOverlay;
class AWeapon;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void DrawHUD() override;

	void SetCrosshairSpread(float Spread);

	void CreateInventorySlot();

	void UpdateInventory();

	void AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon);

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UAOSCharacterOverlay* CharacterOverlay;

	UTexture2D* CrosshairCenter;

	UTexture2D* CrosshairLeft;

	UTexture2D* CrosshairRight;

	UTexture2D* CrosshairTop;

	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> InventorySlotClass;

protected:

	virtual void BeginPlay() override;

	void AddOverlay();

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

private:

	UPROPERTY(EditAnywhere)
	float CrosshairSpread;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

};
