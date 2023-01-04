
#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Types/WeaponType.h"
#include "Types/WeaponState.h"
#include "Weapon.generated.h"

/**
* 무기 아이템 기본 클래스
*/

class UBoxComponent;
class USoundCue;

DECLARE_MULTICAST_DELEGATE_OneParam(OnWeaponStateChangedDelegate, AWeapon* Weapon);

UCLASS()
class ADVENTUREOFSHINBI_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	/** 획득 효과 재생 함수 */
	virtual void PlayGainEffect();

	/** 무기 상태 변경 */
	virtual void SetWeaponState(const EWeaponState State);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	/** 무기 유형 enum class 변수 */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

private:

	/** 무기 상태 enum class 변수 */
	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState = EWeaponState::EWS_MAX;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundCue* PickAndEquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundCue* DropSound;

public:

	/** 무기 상태가 변경됨을 알리는 델리게이트 */
	OnWeaponStateChangedDelegate WeaponStateChanged;

	EWeaponType GetWeaponType() const;
	EWeaponState GetWeaponState() const;

	/** 캐릭터 사망 시 무기 메쉬를 캐릭터 손에서 떨어트리도록 무기 메쉬의 충돌, 물리 관련 설정 */
	void DropWeapon();
};
