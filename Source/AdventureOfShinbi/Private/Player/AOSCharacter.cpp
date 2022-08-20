

#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/AOSHUD.h"

AAOSCharacter::AAOSCharacter()
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

void AAOSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//GetMesh()->HideBoneByName("weapon_r", EPhysBodyOp::PBO_Term);
	OnTakeAnyDamage.AddDynamic(this, &AAOSCharacter::TakeAnyDamage);
}

void AAOSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

void AAOSCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (CombatComp)
	{
		CombatComp->UpdateHealth(Damage);
	}
}

void AAOSCharacter::MoveForward(float Value)
{
	if (bIsAnimationPlaying && WeaponType == EWeaponType::EWT_MAX) return;

	bIsMoving = Value != 0.f ? true : false;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}

}

void AAOSCharacter::MoveRight(float Value)
{
	if (bIsAnimationPlaying && WeaponType == EWeaponType::EWT_MAX) return;

	bIsMoving = Value != 0.f ? true : false;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AAOSCharacter::Lookup(float Value)
{
	AddControllerPitchInput(Value);
}

void AAOSCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AAOSCharacter::Jump()
{
	if (bIsAnimationPlaying) return;

	ACharacter::Jump();
}

void AAOSCharacter::RunningButtonPressed()
{
	if (bIsAnimationPlaying || bCanRunning == false) return;

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = 700.f;
}

void AAOSCharacter::RunningButtonReleased()
{
	if (bIsAnimationPlaying) return;

	bIsRunning = false;

	GetCharacterMovement()->MaxWalkSpeed = 250.f;
}

void AAOSCharacter::CrouchButtonPressed()
{
	if (bIsAnimationPlaying || WeaponType != EWeaponType::EWT_MAX) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AAOSCharacter::EquipButtonPressed()
{
	if (OverlappingWeapon == nullptr || CombatComp == nullptr || OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_MAX) return;

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// TODO : 무기 장착 시 걷기랑 뛰기 속도 값 증가

	/*FName SocketName;
	if (OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_Gun)
	{
		ARangedWeapon* RW = Cast<ARangedWeapon>(OverlappingWeapon);
		GunRecoil = RW->GetGunRecoil();
		SocketName = FName("GunSocket");
	}
	else if (OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_Bow)
	{

	}
	else if (OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_MeleeOneHand)
	{
		SocketName = FName("OneHandSocket");
	}
	else if (OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_MeleeTwoHand)
	{
		SocketName = FName("TwoHandSocket");
	}
	else if (OverlappingWeapon->GetWeaponType() == EWeaponType::EWT_Glave)
	{
		SocketName = FName("GlaveSocket");
	}

	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(OverlappingWeapon, GetMesh());
	}

	WeaponType = OverlappingWeapon->GetWeaponType();

	if (OverlappingWeapon->GetWidget())
	{
		OverlappingWeapon->GetWidget()->SetVisibility(false);
	}

	CombatComp->SetEquippedWeapon(OverlappingWeapon);*/


	CombatComp->PickingUpWeapon(OverlappingWeapon);
	OverlappingWeapon->SetOwner(this);
	OverlappingWeapon = nullptr;
}

void AAOSCharacter::AttackButtonePressed()
{
	if (CombatComp == nullptr || WeaponType == EWeaponType::EWT_MAX) return;

	bAttackButtonPressing = true;

	if (WeaponType == EWeaponType::EWT_Gun)
	{
		Fire();
	}
	else
	{
		CombatComp->MeleeAttack();
	}
}

void AAOSCharacter::AttackButtoneReleassed()
{
	bAttackButtonPressing = false;
}

void AAOSCharacter::Fire()
{
	if (bAbleFire)
	{
		CombatComp->GunFire();
		StartTimerFire();
	}
}

void AAOSCharacter::StartTimerFire()
{
	ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->EquippedWeapon);

	bAbleFire = false;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AAOSCharacter::FireReturn, RW->GetFireRate());

}

void AAOSCharacter::FireReturn()
{
	bAbleFire = true;
	ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->EquippedWeapon);
	if (RW->GetAutomaticFire() && bAttackButtonPressing)
	{
		Fire();
	}
}

void AAOSCharacter::AimButtonPressed()
{
	if (WeaponType == EWeaponType::EWT_Gun)
	{
		bIsAiming = true;
	}
}

void AAOSCharacter::AimButtonReleased()
{
	if (WeaponType == EWeaponType::EWT_Gun)
	{
		bIsAiming = false;
	}
}

void AAOSCharacter::ReloadButtonPressed()
{
	if (WeaponType == EWeaponType::EWT_Gun)
	{
		CombatComp->Reload();
	}
}

void AAOSCharacter::InventoryKeyPressed()
{
	bIsInventoryOn = bIsInventoryOn ? false : true;

	AAOSController* AC = Cast<AAOSController>(Controller);
	if (AC)
	{
		AC->HUDInventoryOn(bIsInventoryOn);
	}
}

void AAOSCharacter::TransitionAnimationStart()
{
	bIsAnimationPlaying = true;
}

void AAOSCharacter::TransitionAnimationEnd()
{
	bIsAnimationPlaying = false;
}

void AAOSCharacter::ResumeRunning()
{
	RunningButtonPressed();
}

void AAOSCharacter::StopRunning()
{
	RunningButtonReleased();
}

void AAOSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAOSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAOSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Lookup", this, &AAOSCharacter::Lookup);
	PlayerInputComponent->BindAxis("Turn", this, &AAOSCharacter::Turn);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAOSCharacter::Jump);
	PlayerInputComponent->BindAction("Running", IE_Pressed, this, &AAOSCharacter::RunningButtonPressed);
	PlayerInputComponent->BindAction("Running", IE_Released, this, &AAOSCharacter::RunningButtonReleased);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAOSCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AAOSCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AAOSCharacter::AttackButtonePressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AAOSCharacter::AttackButtoneReleassed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AAOSCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AAOSCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AAOSCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AAOSCharacter::InventoryKeyPressed);
}

void AAOSCharacter::SetOverlappingWeapon(AWeapon* OtherWeapon)
{
	if (OtherWeapon)
	{
		OverlappingWeapon = OtherWeapon;
	}
}

void AAOSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UCameraComponent* AAOSCharacter::GetCamera() const
{
	return Camera;
}

bool AAOSCharacter::GetIsRunning() const
{
	return bIsRunning;
}

void AAOSCharacter::SetCanRunning(bool IsCapable)
{
	bCanRunning = IsCapable;
}

bool AAOSCharacter::GetIsAnimationPlaying() const
{
	return bIsAnimationPlaying;
}

bool AAOSCharacter::GetIsMoving() const
{
	return bIsMoving;
}

bool AAOSCharacter::GetIsAiming() const
{
	return bIsAiming;
}

bool AAOSCharacter::GetAttackButtonPressing() const
{
	return bAttackButtonPressing;
}

EWeaponType AAOSCharacter::GetCombatState() const
{
	return WeaponType;
}
