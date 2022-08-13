

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

		// RootYawOffset ���� ������Ʈ�ϰ� [-180,180] ���� ����
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);

		// ȸ�� ���̸� 1.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastframe = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastframe };

			// RootYawOffset > 0 : �������� ȸ��, RootYawOffset < 0 : ���������� ȸ��
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

	// MakeFromX���� ���� ȸ������� Rotator�� ����. �� rotator�� ���� ��ǥ�迡�� �Է� �Ķ���� ���͸� X������ �ϴ� ��ǥ��� �����ϱ� ���� �ʿ��� ȸ�����̴�.
	FRotator MovementOffset = UKismetMathLibrary::MakeRotFromX(Velocity);
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementOffset, AimRotation).Yaw;

	if (Velocity.Size() > 0.f)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}

	TurnInPlace();

}
