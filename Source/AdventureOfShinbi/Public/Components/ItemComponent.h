
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ItemType.h"
#include "Types/AmmoType.h"
#include "ItemComponent.generated.h"

class UCombatComponent;
class AAOSCharacter;
class AAOSController;
class AAOSHUD;
class AItem;
class UImage;
class UButton;
class UTextBlock;

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
	UButton* QuickSlotButton;

	UPROPERTY()
	UTextBlock* QuickSlotItemCountText;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UItemComponent();

	void AddItem(AItem* Item);

	void UseActivatedQuickSlotItem();

	void ItemChange();

	void UpdateAmmo(EAmmoType AmmoType);

protected:

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeTMap();

	void InitializeQuickSlotItemArray();

	void AddRecoveryItem(AItem* Item);

	void AddAmmoItem(AItem* Item);

	UFUNCTION()
	void ItemUseOrEquip(AItem* Item, EItemSlotMenuState State);

	void UseItem(AItem* Item);

	UFUNCTION()
	void EquipToItemQuickSlot(int8 SlotIndex, UImage* QuickSlotIcon, UButton* QuickSlotButton, UTextBlock* QuickSlotCountText);

	bool CheckQuickSlotArrayIsEmpty();

	void DismountItem(AItem* Item);

	void PlayQuickSlotAnimation();

	void UseRecoveryItem(AItem* Item);

	void Recovery(float DeltaTime);

	int32 GetItemCount(AItem* Item);

	FSlateBrush SetBrush(float Size, ESlateBrushDrawType::Type DrawType);

	void StaminaRecoveryBoostTimeOff();

private:

	UCombatComponent* CombatComp;

	AAOSCharacter* Character;

	AAOSController* CharacterController;

	AAOSHUD* HUD;

	TArray<AItem*> ItemArray;

	TMap<ERecoveryType, int32> RecoveryItemMap;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float HealthRecoveryAmount = 50.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float HealthRecoveryRate = 50.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float ManaRecoveryAmount = 50.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float ManaRecoveryRate = 10.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float StaminaRecoveryBoostAmount = 2.f;

	TMap<EAmmoType, int32> AmmoQuantityMap;

	TMap<EAmmoType, int8> AmmoIndexMap;

	bool bDoRecoveryHealth = false;
	bool bDoRecoveryMana = false;

	float RecoveredHealthAmount = 0.f;
	float RecoveredManaAmount = 0.f;

	FTimerHandle QuickSlotAnimationTimer;

	float QuickSlotAnimationTime = 1.0f;

	TArray<FQuickSlotItem> QuickSlotItemArray;

	AItem* SelectedItem = nullptr;

	int8 ActivatedQuickSlotNumber = 0;

	FTimerHandle StaminaRecoveryBoostTimer;

	float StaminaRecoveryBoostTime = 60.f;

	float DefaultStaminaIncreaseRate = 0.f;

public:

	void SetCharacter(AAOSCharacter* CharacterToSet);

	void SetCombatComp(UCombatComponent* Combat);

	int32 GetAmmo(EAmmoType Type) const;

	void SetAmmo(EAmmoType Type, int32 Quantity);

};
