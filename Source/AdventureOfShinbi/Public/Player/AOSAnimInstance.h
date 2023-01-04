
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/WeaponType.h"
#include "AOSAnimInstance.generated.h"

/**
 * 
 */

class AAOSCharacter;

UCLASS()
class ADVENTUREOFSHINBI_API UAOSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:

	/** ĳ������ ��Ʈ ���� ��Ʈ�� �����̼� �������� ȸ�� */
	void TurnInPlace();

private:

	UPROPERTY(VisibleAnywhere)
	AAOSCharacter* AOSCharacter;

	/** ĳ���� �ӵ� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/** ĳ���Ͱ� ���߿� �ִ��� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	/** ĳ������ ���� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** ĳ������ �޸��� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsRunning = false;

	/** ĳ������ ��ũ���� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched = false;

	/** ĳ������ �ִϸ��̼� ��� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAnimationPlaying = false;

	/** ĳ������ ������ ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = false;

	/** �������� ���� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

	/** ĳ������ Rotation */
	FRotator CharacterRotation;

	/** ���� Rotation �� ĳ���� Rotation �� ��Ÿ */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float OffsetYaw = 0.f;

	/** ���� Rotation �� Pitch */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Pitch = 0.f;

	float CharacterYaw = 0.f;

	float CharacterYawLastframe = 0.f;

	/** ĳ������ ��Ʈ ���� ȸ���ϱ� ���� Yaw �� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RootYawOffset = 0.f;

	/** TurnInPlace �ִϸ��̼� �������� Ŀ�� �� */
	float RotationCurve = 0.f;

	float RotationCurveLastframe = 0.f;

	/** ĳ������ �̵� �ִϸ��̼� ���� �����̽����� Aixs �� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw = 0.f;

	/** ������ ������ */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsReloading = false;

public:

	void SetWeaponType(EWeaponType Type);
	void ActivateReloading();
	void DeactivateReloading();

};
