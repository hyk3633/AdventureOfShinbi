

#include "Player/PlayerAnimInstance.h"
#include "Player/PlayerCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::TurnInPlace()
{
	if (Speed > 0.f)
	{
		RootYawOffset = 0.f;
		CharacterYaw = CharacterRotation.Yaw;
		CharacterYawLastframe = CharacterYaw;
		RotationCurveLastframe = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastframe = CharacterYaw;
		CharacterYaw = CharacterRotation.Yaw;
		const float YawDelta{ CharacterYaw - CharacterYawLastframe };

		// RootYawOffset 값을 업데이트하고 [-180,180] 으로 고정
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		// 회전 중이면 1.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastframe = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastframe };

			// RootYawOffset > 0 : 왼쪽으로 회전, RootYawOffset < 0 : 오른쪽으로 회전
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float AbsRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (AbsRootYawOffset > 90.f)
			{
				const float YawExcess{ AbsRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}

void UPlayerAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bIsRunning = PlayerCharacter->GetIsRunning();

	bIsCrouched = PlayerCharacter->bIsCrouched;

	bIsAnimationPlaying = PlayerCharacter->GetIsAnimationPlaying();

	bIsMoving = PlayerCharacter->GetIsMoving();

	bIsAiming = PlayerCharacter->GetIsAiming();

	CombatType = PlayerCharacter->GetCombatState();

	FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
	CharacterRotation = PlayerCharacter->GetActorRotation();

	Pitch = AimRotation.Pitch;

	OffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, CharacterRotation).Yaw;

	// MakeFromX에서 만든 회전행렬의 Rotator를 리턴. 이 rotator는 월드 좌표계에서 입력 파라미터 벡터를 X축으로 하는 좌표계로 이전하기 위해 필요한 회전각이다.
	FRotator MovementOffset = UKismetMathLibrary::MakeRotFromX(Velocity);
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementOffset, AimRotation).Yaw;

	if (Velocity.Size() > 0.f)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}

	TurnInPlace();

}
