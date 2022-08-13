

#include "Player/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CombatComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	SpringArm->bUsePawnControlRotation = true; // 폰의 뷰/컨트롤 회전 값 사용
	SpringArm->SocketOffset = FVector(0.f, 50.f, 50.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bOrientRotationToMovement = true; // 가속되는 방향으로 캐릭터를 회전
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->HideBoneByName("weapon_r", EPhysBodyOp::PBO_Term);

}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (bIsAnimationPlaying && CombatType == EWeaponType::EWT_MAX) return;

	bIsMoving = Value != 0.f ? true : false;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}

}

void APlayerCharacter::MoveRight(float Value)
{
	if (bIsAnimationPlaying && CombatType == EWeaponType::EWT_MAX) return;

	bIsMoving = Value != 0.f ? true : false;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::Lookup(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::Jump()
{
	if (bIsAnimationPlaying) return;

	ACharacter::Jump();
}

void APlayerCharacter::RunningButtonPressed()
{
	if (bIsAnimationPlaying) return;

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = 700.f;
}

void APlayerCharacter::RunningButtonReleased()
{
	if (bIsAnimationPlaying) return;

	bIsRunning = false;

	GetCharacterMovement()->MaxWalkSpeed = 250.f;
}

void APlayerCharacter::CrouchButtonPressed()
{
	if (bIsAnimationPlaying || CombatType != EWeaponType::EWT_MAX) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::EquipButtonPressed()
{
	if (OverlappingWeapon == nullptr || CombatComp == nullptr || OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_MAX) return;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// TODO : 무기 장착 시 걷기랑 뛰기 속도 값 증가

	if (GetMesh() == nullptr) return;

	FName SocketName;
	switch (OverlappingWeapon->GetWeaponType())
	{
	case EWeaponType::EWT_Gun:
		SocketName = FName("RightHandGunSocket");
		break;

	case EWeaponType::EWT_Bow:
		break;

	case EWeaponType::EWT_MeleeOneHand:
		SocketName = FName("RightHandMeleeOneHandSocket");
		break;

	case EWeaponType::EWT_MeleeTwoHand:
		SocketName = FName("RightHandMeleeTwoHandSocket");
		break;

	case EWeaponType::EWT_Glave:
		SocketName = FName("LeftHandGlaveSocket");
		break;
	}
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(OverlappingWeapon, GetMesh());
	}

	CombatType = OverlappingWeapon->GetWeaponType();

	CombatComp->SetEquippedWeapon(OverlappingWeapon);
}

void APlayerCharacter::AttackButtonePressed()
{
	if (CombatComp && CombatType != EWeaponType::EWT_MAX)
	{
		CombatComp->Attack();
	}
}

void APlayerCharacter::AttackButtoneReleassed()
{

}

void APlayerCharacter::AimButtonPressed()
{
	if (CombatType == EWeaponType::EWT_Gun)
	{
		bIsAiming = true;
	}
}

void APlayerCharacter::AimButtonReleased()
{
	if (CombatType == EWeaponType::EWT_Gun)
	{
		bIsAiming = false;
	}
}

void APlayerCharacter::TransitionAnimationStart()
{
	bIsAnimationPlaying = true;
}

void APlayerCharacter::TransitionAnimationEnd()
{
	bIsAnimationPlaying = false;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Lookup", this, &APlayerCharacter::Lookup);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Running", IE_Pressed, this, &APlayerCharacter::RunningButtonPressed);
	PlayerInputComponent->BindAction("Running", IE_Released, this, &APlayerCharacter::RunningButtonReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APlayerCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::AttackButtonePressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &APlayerCharacter::AttackButtoneReleassed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::AimButtonReleased);
}

void APlayerCharacter::SetOverlappingWeapon(AWeapon* OtherWeapon)
{
	if (OtherWeapon)
	{
		OverlappingWeapon = OtherWeapon;
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
