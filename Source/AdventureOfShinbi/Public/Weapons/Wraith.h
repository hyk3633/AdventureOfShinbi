
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Wraith.generated.h"

class AAOSCharacter;

/** 무기 스켈레탈 메쉬의 본 Location, Rotation 구조체 */
USTRUCT(BlueprintType)
struct FWeaponPartsLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Barrel;							 
	UPROPERTY(BlueprintReadWrite)
	FVector Scope;							 
	UPROPERTY(BlueprintReadWrite)
	FVector Lens;							 
	UPROPERTY(BlueprintReadWrite)
	FVector StockBott;						 
	UPROPERTY(BlueprintReadWrite)
	FRotator StockBottRot;					 
	UPROPERTY(BlueprintReadWrite)
	FVector StockTop;
};

UCLASS()
class ADVENTUREOFSHINBI_API AWraith : public ARangedProjectileWeapon
{
	GENERATED_BODY()
	
public:

	AWraith();

	/** 발사 함수 */
	virtual void Firing() override;

	virtual void SetWeaponState(const EWeaponState State) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 스켈레탈 메쉬의 본 Location, Rotation 값 보간 */
	void InterpWeaponPartsLocation(float DeltaTime, bool bSniperMode);

	/** 무기 형태 변환 */
	UFUNCTION()
	void FormChange(bool bSniperMode);

	void ActivateScopeParticle();

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	TSubclassOf<UCameraShakeBase> CameraShakeAimingShot;

	/** 스나이퍼 모드의 투사체 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	TSubclassOf<AProjectile> AimingProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedMuzzleFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedSmokeParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* AimedStabilizerParticle;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* AimedFireSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	UParticleSystem* ScopeParticle;

	UParticleSystemComponent* ScopeParticleComp;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* ActivateSound;

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	USoundCue* DeactivateSound;

	/** 조준 중일 경우 true */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAimed = false;

	/** 무기의 형태 변환 속도 */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	float FormChangeSpeed = 5.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponPartsLocation CurrentLocation;

	FWeaponPartsLocation AssaultMode;

	FWeaponPartsLocation SniperMode;

};
