
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/AmmoType.h"
#include "Types/ItemType.h"
#include "AOSGameModeBase.generated.h"

/**
 * ���� ��� ���̽� Ŭ����
 * �÷��̾� ��� ó��
 * ���� �й� ���� ó��
 * �÷��̾� ������ �� ������ �ҷ�����
 * �����ν��Ͻ��� ������ ���� �� �ҷ�����
 */

class UAOSGameInstance;
class AAOSCharacter;
class AAOSController;
class UItemComponent;
class UCombatComponent;
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

/** ������ ������ ���� ����ü */
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

	friend AAOSController;

	friend UItemComponent;

	friend UCombatComponent;

	friend UAOSGameInstance;

	AAOSGameModeBase();

	void RespawnPlayer();

	bool IsPlayerRespawn();

	void LoadPlayerData();

	void BindBossDefeatEvent();

	void InitializeTMap();

	void InitializeQuickSlotItemArray();

	int8 GetRecoveryIndex(ERecoveryType Type) const;

	void SetQuickSlotItem(int32 Index, AItem* Item, UImage* Icon, UTextBlock* CountText);

	void ClearQuickSlot(int32 Index);

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

	UPROPERTY()
	UAOSGameInstance* GI;

	UPROPERTY()
	AAOSCharacter* Character;

	UPROPERTY()
	AAOSController* CharacterController;

	/** �÷��̾ ������ �� �÷��̾� ��ŸƮ */
	UPROPERTY()
	APlayerStart* PlayerStart;

	/** �÷��̾� �����͸� ������ ������ */

	UPROPERTY()
	AWeapon* EquippedWeapon = nullptr;

	UPROPERTY()
	AWeapon* QuickSlot1Weapon = nullptr;
	UPROPERTY()
	AWeapon* QuickSlot2Weapon = nullptr;

	UPROPERTY()
	TArray<AWeapon*> AcquiredWeapons;

	UPROPERTY()
	TArray<AItem*> ItemArray;

	TMap<ERecoveryType, int32> RecoveryItemMap;

	TMap<ERecoveryType, int8> RecoveryIndexMap;

	TMap<EAmmoType, int32> AmmoQuantityMap;

	TMap<EAmmoType, int8> AmmoIndexMap;

	TArray<FQuickSlotItem> QuickSlotItemArray;

	int8 ActivatedQuickSlotNumber = 0;

	int32 PlayerDeathCount = 0;

	/** ������ ���ø� Ŭ������ */

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

	/** �÷��̾� ��� ����, ���� ���� */

	UPROPERTY(EditAnywhere, Category = "Game Mode | Player Death")
	TSubclassOf<UUserWidget> DeathSignWidgetClass;

	UPROPERTY()
	UUserWidget* DeathSignWidget;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Player Death")
	USoundCue* DeathSound;

	FTimerHandle DeathSignTimer;

	/** ���� �й� ����, ���� ���� */

	UPROPERTY(EditAnywhere, Category = "Game Mode | Boss Defeated")
	TSubclassOf<UUserWidget> BossDefeatedSignWidgetClass;

	UPROPERTY()
	UUserWidget* BossDefeatedSignWidget;

	UPROPERTY(EditAnywhere, Category = "Game Mode | Boss Defeated")
	USoundCue* BossDefeatedSound;

	FTimerHandle BossDefeatedSignTimer;
};
