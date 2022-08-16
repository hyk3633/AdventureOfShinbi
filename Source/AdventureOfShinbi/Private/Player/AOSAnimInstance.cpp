// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AOSAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAOSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AOSCharacter = Cast<AAOSCharacter>(TryGetPawnOwner());
}

void UAOSAnimInstance::TurnInPlace()
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

void UAOSAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (AOSCharacter == nullptr) return;

	FVector Velocity = AOSCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = AOSCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = AOSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bIsRunning = AOSCharacter->GetIsRunning();

	bIsCrouched = AOSCharacter->bIsCrouched;

	bIsAnimationPlaying = AOSCharacter->GetIsAnimationPlaying();

	bIsMoving = AOSCharacter->GetIsMoving();

	bIsAiming = AOSCharacter->GetIsAiming();

	WeaponType = AOSCharacter->GetCombatState();

	FRotator AimRotation = AOSCharacter->GetBaseAimRotation();
	CharacterRotation = AOSCharacter->GetActorRotation();

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