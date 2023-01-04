
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

	/** 캐릭터의 루트 본을 컨트롤 로테이션 방향으로 회전 */
	void TurnInPlace();

private:

	UPROPERTY(VisibleAnywhere)
	AAOSCharacter* AOSCharacter;

	/** 캐릭터 속도 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Speed;

	/** 캐릭터가 공중에 있는지 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	/** 캐릭터의 가속 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** 캐릭터의 달리기 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsRunning = false;

	/** 캐릭터의 웅크리기 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched = false;

	/** 캐릭터의 애니메이션 재생 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAnimationPlaying = false;

	/** 캐릭터의 정조준 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = false;

	/** 장착중인 무기 유형 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

	/** 캐릭터의 Rotation */
	FRotator CharacterRotation;

	/** 에임 Rotation 과 캐릭터 Rotation 의 델타 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float OffsetYaw = 0.f;

	/** 에임 Rotation 의 Pitch */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Pitch = 0.f;

	float CharacterYaw = 0.f;

	float CharacterYawLastframe = 0.f;

	/** 캐릭터의 루트 본을 회전하기 위한 Yaw 값 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RootYawOffset = 0.f;

	/** TurnInPlace 애니메이션 시퀀스의 커브 값 */
	float RotationCurve = 0.f;

	float RotationCurveLastframe = 0.f;

	/** 캐릭터의 이동 애니메이션 블렌드 스페이스에서 Aixs 값 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw = 0.f;

	/** 재장전 중인지 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsReloading = false;

public:

	void SetWeaponType(EWeaponType Type);
	void ActivateReloading();
	void DeactivateReloading();

};
