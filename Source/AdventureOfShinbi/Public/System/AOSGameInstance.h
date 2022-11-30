
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Types/WeaponType.h"
#include "Types/ItemType.h"
#include "Types/AmmoType.h"
#include "AOSGameInstance.generated.h"

/**
 * 
 */

UENUM()
enum class ESlotType : uint8
{
	EST_None,
	EST_Equip,
	EST_Quick1,
	EST_Quick2,

	EST_MAX
};

USTRUCT()
struct FWeaponInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	EWeaponType WeaponType;

	UPROPERTY()
	ESlotType SlotType;

	UPROPERTY()
	int32 LoadedAmmo;

};

USTRUCT()
struct FItemInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	int8 ItemDetailType;

	UPROPERTY()
	int32 ItemCount;

	UPROPERTY()
	int8 QuickSlotIndex;

	UPROPERTY()
	bool bIsEquipped;

};

UCLASS()
class ADVENTUREOFSHINBI_API UAOSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	void SavePlayerData();

	FWeaponInfo GetWeaponInfo(int32 Index);

	FItemInfo GetItemInfo(int32 Index);

	int32 GetWeaponInfoCount();

	int32 GetItemInfoCount();

	float GetCurrentHealth() const;
	float GetCurrentMana() const;
	bool GetIsArmed() const;

	void AcitavateShouldLoadData();
	void DeacitavateShouldLoadData();
	bool GetShouldLoadData() const;

protected:

	void SavePlayerInfo();

	void SaveWeaponInfo();

	void SaveItemInfo();

private:

	TArray<FWeaponInfo> WeaponInfoArr;

	TArray<FItemInfo> ItemInfoArr;

	float CurrentHealth = 0.f;
	float CurrentMana = 0.f;
	bool bIsArmed = false;

	bool bShouldLoadData = false;
};
