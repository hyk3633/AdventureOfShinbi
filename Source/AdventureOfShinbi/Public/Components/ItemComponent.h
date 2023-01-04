
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

	/** 플레이어 리스폰 후 컴포넌트를 이전 상태로 초기화 */
	void RestartItemComp();

	/** 아이템 추가 */
	void AddItem(AItem* Item);

	/** 퀵슬롯 활성화 아이템 사용 */
	void UseActivatedQuickSlotItem();

	/** 탄약 아이템 추가 */
	void AddAmmoItem(AItem* Item);

	/** 아이템 상태에 따라 처리 */
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

	/** 스탯 회복량 및 회복율 */

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float HealthRecoveryAmount = 50.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float HealthRecoveryRate = 1000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float ManaRecoveryAmount = 1000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float ManaRecoveryRate = 500.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.0, ClampMax = 1000.0))
	float StaminaRecoveryBoostAmount = 2.f;

	/** 회복 진행 여부 */
	bool bDoRecoveryHealth = false;
	bool bDoRecoveryMana = false;

	/** 회복된 양 */
	float RecoveredHealthAmount = 0.f;
	float RecoveredManaAmount = 0.f;

	FTimerHandle StaminaRecoveryBoostTimer;

	/** 지구력 회복 속도 증가 시간 */
	float StaminaRecoveryBoostTime = 60.f;

	float DefaultStaminaIncreaseRate = 0.f;

public:

	void SetCharacter(AAOSCharacter* CharacterToSet);

	void SetCombatComp(UCombatComponent* Combat);

	void SetController(AAOSController* Cont);
	void SetGameMode(AAOSGameModeBase* Gamemode);
};
