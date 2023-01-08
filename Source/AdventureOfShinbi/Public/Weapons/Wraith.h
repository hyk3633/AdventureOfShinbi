
#pragma once

#include "CoreMinimal.h"
#include "Weapons/RangedProjectileWeapon.h"
#include "Wraith.generated.h"

class AAOSCharacter;

/** ���� ���̷�Ż �޽��� �� Location, Rotation ����ü */
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

	/** �߻� �Լ� */
	virtual void Firing() override;

	virtual void SetWeaponState(const EWeaponState State) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** ���̷�Ż �޽��� �� Location, Rotation �� ���� */
	void InterpWeaponPartsLocation(float DeltaTime, bool bSniperMode);

	/** ���� ���� ��ȯ */
	UFUNCTION()
	void FormChange(bool bSniperMode);

	void ActivateScopeParticle();

private:

	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	TSubclassOf<UCameraShakeBase> CameraShakeAimingShot;

	/** �������� ����� ����ü ���ø� Ŭ���� */
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

	/** ���� ���� ��� true */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAimed = false;

	/** ������ ���� ��ȯ �ӵ� */
	UPROPERTY(EditAnywhere, Category = "Ranged Weapon | Aimed")
	float FormChangeSpeed = 5.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FWeaponPartsLocation CurrentLocation;

	FWeaponPartsLocation AssaultMode;

	FWeaponPartsLocation SniperMode;

};
