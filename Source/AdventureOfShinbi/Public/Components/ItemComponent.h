
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ItemType.h"
#include "Types/AmmoType.h"
#include "ItemComponent.generated.h"

class UCombatComponent;
class AAOSCharacter;
class AAOSGameModeBase;
class AAOSController;
class AItem;
class UImage;
class UButton;
class UTextBlock;
class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UItemComponent();

	void RestartItemComp();

	void AddItem(AItem* Item);

	void UseActivatedQuickSlotItem();

	void AddAmmoItem(AItem* Item);

	UFUNCTION()
	void ItemUseOrEquip(AItem* Item, EItemSlotMenuState State);

protected:

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddRecoveryItem(AItem* Item);

	void UseItem(AItem* Item);

	void DismountItem(AItem* Item);

	void PlayQuickSlotAnimation();

	void UseRecoveryItem(AItem* Item);

	void Recovery(float DeltaTime);

	int32 GetItemCount(AItem* Item);

	void StaminaRecoveryBoostTimeOff();

private:

	UCombatComponent* CombatComp;

	AAOSCharacter* Character;

	AAOSGameModeBase* GameMode;

	AAOSController* CharacterController;

	UPROPERTY(EditAnywhere, Category = "Item Compnent | Voice")
	USoundCue* VoiceRecoveryHealth;

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

	bool bDoRecoveryHealth = false;
	bool bDoRecoveryMana = false;

	float RecoveredHealthAmount = 0.f;
	float RecoveredManaAmount = 0.f;

	FTimerHandle StaminaRecoveryBoostTimer;

	float StaminaRecoveryBoostTime = 60.f;

	float DefaultStaminaIncreaseRate = 0.f;

public:

	void SetCharacter(AAOSCharacter* CharacterToSet);

	void SetCombatComp(UCombatComponent* Combat);

	void SetController(AAOSController* Cont);

};
