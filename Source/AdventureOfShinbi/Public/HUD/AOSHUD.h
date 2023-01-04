
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AOSHUD.generated.h"

/**
 * HUD 클래스
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
	// 인벤토리
	//

	/** 인벤토리 슬롯 확장 */
	void CreateInventorySlot();

	/** 인벤토리 슬롯 개수 반환 */
	int32 GetInventorySlotCount();

	/** 인벤토리 슬롯 갱신 */
	void UpdateInventory();

	/** 슬롯에 무기 아이템 추가 */
	void AddWeaponToSlot(int32 SlotNum, AWeapon* Weapon);

	/////////////////////////
	// 아이템 인벤토리
	//

	/** 아이템 인벤토리 슬롯 확장 */
	void CreateItemInventorySlot();

	/** 아이템 인벤토리 슬롯 갱신 */
	void UpdateItemInventory();

	/** 아이템 슬롯에 아이템 추가 */
	void AddItemToSlot(int32 SlotNum, AItem* Item);

	/** 아이템 인벤토리 슬롯 개수 반환 */
	int32 GetItemInventorySlotCount();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	/** 오버레이 위젯을 뷰포트에 추가 */
	void AddOverlay();

	/** 크로스헤어 표시 */
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

public:

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY(BlueprintReadOnly)
	UAOSCharacterOverlay* CharacterOverlay;

	/** 크로스헤어 텍스쳐 */

	UTexture2D* CrosshairCenter;

	UTexture2D* CrosshairLeft;

	UTexture2D* CrosshairRight;

	UTexture2D* CrosshairTop;

	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> InventorySlotClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> ItemInventorySlotClass;

	/** 크로스헤어 퍼짐 계수 */
	UPROPERTY(EditAnywhere)
	float CrosshairSpread;

private:

	/** 크로스헤어 퍼짐 기본값 */
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

};
