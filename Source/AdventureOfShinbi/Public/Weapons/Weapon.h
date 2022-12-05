
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/WeaponType.h"
#include "Types/WeaponState.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USoundCue;

DECLARE_MULTICAST_DELEGATE_OneParam(OnWeaponStateChangedDelegate, AWeapon* Weapon);

UCLASS()
class ADVENTUREOFSHINBI_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	virtual void PlayGainEffect();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

private:

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState = EWeaponState::EWS_MAX;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundCue* PickAndEquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundCue* DropSound;

public:

	OnWeaponStateChangedDelegate WeaponStateChanged;

	EWeaponType GetWeaponType() const;
	virtual void SetWeaponState(const EWeaponState State);
	EWeaponState GetWeaponState() const;
	void DropWeapon();
};
