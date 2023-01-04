
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AOSHUD.generated.h"

/**
 * HUD Ŭ����
 */

class UUserWidget;
class UAOSCharacterOverlay;
class AWeapon;
class AItem;

UCLASS()
class ADVENTUREOFSHINBI_API AAOSHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void DrawHUD() override;

	/////////////////////////
	// �κ��丮
	//

	/** �κ��丮 ���� Ȯ�� */
	void CreateInventorySlot();

	/** �κ��丮 ���� ���� ��ȯ */
	int32 GetInventorySlotCount();

	/** �κ��丮 ���� ���� */
	void UpdateInventory();

	/** ���Կ� ���� ������ �߰� */
	void AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon);

	/////////////////////////
	// ������ �κ��丮
	//

	/** ������ �κ��丮 ���� Ȯ�� */
	void CreateItemInventorySlot();

	/** ������ �κ��丮 ���� ���� */
	void UpdateItemInventory();

	/** ������ ���Կ� ������ �߰� */
	void AddItemToSlot(int32 SlotNum, AItem* Item);

	/** ������ �κ��丮 ���� ���� ��ȯ */
	int32 GetItemInventorySlotCount();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	/** �������� ������ ����Ʈ�� �߰� */
	void AddOverlay();

	/** ũ�ν���� ǥ�� */
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

public:

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY(BlueprintReadOnly)
	UAOSCharacterOverlay* CharacterOverlay;

	/** ũ�ν���� �ؽ��� */

	UTexture2D* CrosshairCenter;

	UTexture2D* CrosshairLeft;

	UTexture2D* CrosshairRight;

	UTexture2D* CrosshairTop;

	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> InventorySlotClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> ItemInventorySlotClass;

	/** ũ�ν���� ���� ��� */
	UPROPERTY(EditAnywhere)
	float CrosshairSpread;

private:

	/** ũ�ν���� ���� �⺻�� */
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

};
