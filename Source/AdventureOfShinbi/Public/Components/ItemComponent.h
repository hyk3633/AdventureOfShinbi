
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOFSHINBI_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UItemComponent();

	void AddItem(AItem* Item);

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeTMap();

	void AddRecoveryItem(AItem* Item);

	void AddAmmoItem(AItem* Item);

	UFUNCTION()
	void ItemUse(AItem* Item);

	void UseRecoveryItem(AItem* Item);

	void RecoveryHealth(float DeltaTime);

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
	float StaminaRecoveryRate = 10.f;

	TMap<EAmmoType, int32> AmmoQuantityMap;

	bool bDoRecoveryHealth = false;

	float RecoveredHealthAmount = 0.f;

public:

	void SetCharacter(AAOSCharacter* CharacterToSet);

	void SetCombatComp(UCombatComponent* Combat);

	TMap<EAmmoType, int32> GetAmmoMap() const;

};
