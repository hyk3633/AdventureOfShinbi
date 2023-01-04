
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/AmmoType.h"
#include "Types/ItemType.h"
#include "AOSGameModeBase.generated.h"

/**
 * 게임 모드 베이스 클래스
 * 플레이어 사망 처리
 * 보스 패배 관련 처리
 * 플레이어 리스폰 후 데이터 불러오기
 * 게임인스턴스에 데이터 저장 및 불러오기
 */

class UAOSGameInstance;
class AAOSCharacter;
class AAOSController;
class APlayerStart;
class UTexture2D;
class AWeapon;
class AItem;
class UImage;
class UButton;
class UTextBlock;
class AShinbiSword;
class AGlaive;
class ARangedHitScaneWeapon;
class AWraith;
class ARevenent;
class AItemRecovery;
class AItemAmmo;
class USoundCue;

DECLARE_MULTICAST_DELEGATE(PlayerRespawnDelegate);

/** 퀵슬롯 아이템 정보 구조체 */
USTRUCT()
struct FQuickSlotItem
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AItem* QuickSlotItem;

	UPROPERTY()
	UImage* QuickSlotIcon;

	UPROPERTY()
	UTextBlock* QuickSlotItemCountText;

};

UCLASS()
class ADVENTUREOFSHINBI_API AAOSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AAOSGameModeBase();

	void RespawnPlayer();

	bool IsPlayerRespawn();

	void LoadPlayerData();

	void BindBossDefeatEvent();

	/** 무기 데이터 처리 함수 */

	void AddWeaponToArr(AWeapon* Weapon);

	void RemoveWeaponFromArr(AWeapon* Weapon);

	void KeepEquippedWeapon(AWeapon* Weapon);

	void KeepQuickSlot1Weapon(AWeapon* Weapon);

	void KeepQuickSlot2Weapon(AWeapon* Weapon);

	int32 GetWeaponCount() const;

	AWeapon* GetWeaponItem(int32 Index) const;

	AWeapon* GetEquippedWeapon();
	AWeapon* GetQuickSlot1Weapon();
	AWeapon* GetQuickSlot2Weapon();

	/** 아이템 데이터 처리 함수 */

	void InitializeTMap();

	void InitializeQuickSlotItemArray();

	AItem* GetItem(int32 Index) const;

	void AddItemToArr(AItem* Item);

	void RemoveItemFromArr(AItem* Item);

	int32 GetItemIndex(AItem* Item) const;

	int32 GetItemCount() const;

	int32 GetRecoveryItemCount(ERecoveryType Type) const;

	void AddRecoveryItem(ERecoveryType Type, int32 Quantity);

	int8 GetRecoveryIndex(ERecoveryType Type) const;

	void SetRecoveryIndex(ERecoveryType Type, int8 Index);

	void AddAmmoToAmmoMap(EAmmoType Type, int32 AmmoQuantity);

	void AddAmmoQuantity(EAmmoType Type, int32 AmmoQuantity);

	int32 GetAmmoQuantity(EAmmoType Type) const;

	bool IsAmmoTypeExist(EAmmoType Type);

	int32 GetAmmoIndex(EAmmoType Type) const;

	void SetAmmoIndex(EAmmoType Type, int32 Count);

	AItem* GetQuickSlotItem(int8 Index);

	void SetQuickSlotItem(int32 Index, AItem* Item, UImage* Icon, UTextBlock* CountText);

	void SetQuickSlotCountText(int32 Index, FText Text);

	void ClearQuickSlot(int32 Index);

	int32 GetQuickSlotItemArrLength() const;

	FQuickSlotItem GetQuickSlotItemArr(int32 Index);

	int8 GetActivatedQuickSlotNumber() const;

	void SetActivatedQuickSlotNumber(int8 Number);

	void SetPlayerStart(APlayerStart* RespawnPoint);

	PlayerRespawnDelegate DPlayerRespawn;

protected:

	virtual void BeginPlay() override;

	void LoadPlayerInfo();

	void LoadWeaponInfo();

	void LoadItemInfo();

	UFUNCTION()
	void ShowDeathSignWidget();

	void DeathSignDestroy();

	UFUNCTION()
	void ShowBossDefeatedSignWidget();

	void BossDefeatedSignDestroy();

private:

	UAOSGameInstance* GI;

	AAOSCharacter* Character;

	AAOSController* CharacterController;

	/** 플레이어가 리스폰 될 플레이어 스타트 */
	APlayerStart* PlayerStart;

	/** 플레이어 데이터를 저장한 변수들 */

	AWeapon* EquippedWeapon = nullptr;

	AWeapon* QuickSlot1Weapon = nullptr;
	AWeapon* QuickSlot2Weapon = nullptr;

	TArray<AWeapon*> AcquiredWeapons;

	TArray<AItem*> ItemArray;

	TMap<ERecoveryType, int32> RecoveryItemMap;

	TMap<ERecoveryType, int8> RecoveryIndexMap;

	TMap<EAmmoType, int32> AmmoQuantityMap;

	TMap<EAmmoType, int8> AmmoIndexMap;

	TArray<FQuickSlotItem> QuickSlotItemArray;

	int8 ActivatedQuickSlotNumber = 0;

	int32 PlayerDeathCount = 0;

	/** 아이템 템플릿 클래스들 */

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AWeapon> ShinbiSwordClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AWeapon> GlaiveClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AWeapon> AK47Class;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AWeapon> WraithClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AWeapon> RevenentClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> HealthRecoveryClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> ManaRecoveryClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> StaminaRecoveryClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> ARAmmoClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> ShotgunAmmoClass;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Item Class")
	TSubclassOf<AItem> SniperAmmoClass;

	/** 플레이어 사망 위젯, 사운드 변수 */

	UPROPERTY(EditAnywhere, Category = "Game Mode | Player Death")
	TSubclassOf<UUserWidget> DeathSignWidgetClass;

	UUserWidget* DeathSignWidget;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Player Death")
	USoundCue* DeathSound;

	FTimerHandle DeathSignTimer;

	/** 보스 패배 위젯, 사운드 변수 */

	UPROPERTY(EditAnywhere, Category = "Game Mode | Boss Defeated")
	TSubclassOf<UUserWidget> BossDefeatedSignWidgetClass;

	UUserWidget* BossDefeatedSignWidget;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Boss Defeated")
	USoundCue* BossDefeatedSound;

	FTimerHandle BossDefeatedSignTimer;
};
