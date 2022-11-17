

#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "Player/AOSAnimInstance.h"
#include "Enemy/EnemyCharacter.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Weapons/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "HUD/AOSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

AAOSCharacter::AAOSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Player);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyWeaponTrace, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyAttackRange, ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerWeaponTrace, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponTrace, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttackRange, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ItemRange, ECollisionResponse::ECR_Overlap);

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
	GetCharacterMovement()->MaxWalkSpeed = OriginWalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = OriginCrouchedSpeed;

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));

	ItemComp = CreateDefaultSubobject<UItemComponent>(TEXT("Item Component"));
}

void AAOSCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UAOSAnimInstance>(GetMesh()->GetAnimInstance());

	//GetMesh()->HideBoneByName("weapon_r", EPhysBodyOp::PBO_Term);
	OnTakePointDamage.AddDynamic(this, &AAOSCharacter::TakePointDamage);
	OnTakeRadialDamage.AddDynamic(this, &AAOSCharacter::TakeRadialDamage);

}

void AAOSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetOverlappingItem();
}

void AAOSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->Character = this;
		CombatComp->ItemComp = ItemComp;
	}
	if (ItemComp)
	{
		ItemComp->SetCharacter(this);
		ItemComp->SetCombatComp(CombatComp);
	}
}

void AAOSCharacter::TakeAnyDamage(AActor* DamagedActor, float DamageReceived, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("any damage %f"), DamageReceived);
	if (CombatComp)
	{
		CombatComp->UpdateHealth(DamageReceived);
	}
}

void AAOSCharacter::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("point damage %f"), DamageReceived);
	if (CombatComp)
	{
		CombatComp->UpdateHealth(DamageReceived);
	}

	if (CharacterState == ECharacterState::ECS_Nothing)
	{
		AProjectile* Proj = Cast<AProjectile>(DamageCauser);
		if (Proj)
		{
			if (FMath::RandRange(0.f, 1.f) <= 0.2f)
			{
				CombatComp->PlayHitReactMontage();
				PlayerKnockBack(DamageCauser, 1000.f);
			}
		}
		else
		{
			CombatComp->PlayHitReactMontage();
			PlayerKnockBack(DamageCauser, 3000.f);
		}
	}

	PlayHitEffect(HitLocation, ShotFromDirection.Rotation());
}

void AAOSCharacter::PlayerKnockBack(AActor* DamageCauser, float Power)
{
	FVector Direction = GetActorLocation() - DamageCauser->GetActorLocation();
	Direction.Normalize();
	LaunchCharacter(Direction * Power, false, true);
}

void AAOSCharacter::PlayHitEffect(FVector HitLocation, FRotator HitRotation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, HitRotation);
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
	}
}

void AAOSCharacter::TakeRadialDamage(AActor* DamagedActor, float DamageReceived, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("radial %f"), DamageReceived);
	if (CombatComp)
	{
		CombatComp->UpdateHealth(DamageReceived);
	}
}

void AAOSCharacter::MoveForward(float Value)
{
	if (CharacterState != ECharacterState::ECS_Nothing || bDashing)
		return;

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
	if (CharacterState != ECharacterState::ECS_Nothing || bDashing)
		return;

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
	if (CharacterState != ECharacterState::ECS_Nothing || bDashing)
		return;

	MakeNoise(1.f, GetInstigator(), GetActorLocation());

	ACharacter::Jump();
}

void AAOSCharacter::RunningButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bDashing || bCanRunning == false || GetCharacterMovement()->GetCurrentAcceleration().Size() <= 0.f)
		return;

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = CurrentRunningSpeed;
}

void AAOSCharacter::RunningButtonReleased()
{
	bIsRunning = false;

	GetCharacterMovement()->MaxWalkSpeed = CurrentWalkingSpeed;
}

void AAOSCharacter::Crouch_DashButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (WeaponType == EWeaponType::EWT_None)
	{
		Crouching();
	}
	else
	{
		Dash();
	}
}

void AAOSCharacter::Crouching()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AAOSCharacter::Dash()
{
	if (bDashing || !CombatComp->SpendStamina(10.f)) 
		return;
	bDashing = true;
	PlayDashMotage();
	LaunchCharacter(GetLastMovementInputVector() * DashPower, false, false);
}

void AAOSCharacter::Equip_Skill1ButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (OverlappingItem)
	{
		// 무기 장착시에 설정하고 무기 빼면 원상복구하기
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		CombatComp->PickingUpItem(OverlappingItem);
		OverlappingItem = nullptr;
	}
	else if (CombatComp->EquippedWeapon)
	{
		CombatComp->WeaponSkill1();
	}
}

void AAOSCharacter::AttackButtonPressed()
{
	if (CombatComp == nullptr || WeaponType == EWeaponType::EWT_MAX || CharacterState != ECharacterState::ECS_Nothing)
		return;

	bAttackButtonPressing = true;

	if (CombatComp->EquippedWeapon)
	{
		DAttackButtonPressed.ExecuteIfBound();
	}

	if (WeaponType == EWeaponType::EWT_Gun)
	{
		Fire();
	}
	else
	{
		CombatComp->MeleeAttack();
	}
}

void AAOSCharacter::AttackButtonReleassed()
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

void AAOSCharacter::PlayDashMotage()
{
	if (DashMontage)
	{
		AnimInstance->Montage_Play(DashMontage);
	}
}

void AAOSCharacter::OnDashMontageEnded()
{
	bDashing = false;
}

void AAOSCharacter::AimButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (CombatComp->EquippedWeapon)
	{
		DAimButtonPressed.ExecuteIfBound(true);

		if (WeaponType == EWeaponType::EWT_Gun)
		{
			bIsAiming = true;
		}
		else if (WeaponType == EWeaponType::EWT_MeleeOneHand)
		{
			CombatComp->WeaponRightClickSkill();
		}
	}
}

void AAOSCharacter::AimButtonReleased()
{
	if (CombatComp->EquippedWeapon)
	{
		DAimButtonPressed.ExecuteIfBound(false);
	}

	if (WeaponType == EWeaponType::EWT_Gun)
	{
		bIsAiming = false;
	}
}

void AAOSCharacter::Reload_Skill2ButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	CombatComp->WeaponSkill2();
}

void AAOSCharacter::InventoryKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	bIsInventoryOn = bIsInventoryOn ? false : true;

	AAOSController* AC = Cast<AAOSController>(Controller);
	if (AC)
	{
		AC->HUDInventoryOn(bIsInventoryOn);
	}
}

void AAOSCharacter::WeaponQuickSwapKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (CombatComp)
	{
		CombatComp->WeaponQuickSwap();
	}
}

void AAOSCharacter::UseItemKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (ItemComp)
	{
		ItemComp->UseActivatedQuickSlotItem();
	}
}

void AAOSCharacter::ItemChangeKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (ItemComp)
	{
		ItemComp->ItemChange();
	}
}

void AAOSCharacter::Skill3ButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (CombatComp->EquippedWeapon)
	{
		CombatComp->WeaponSkill3();
	}
}

void AAOSCharacter::TransitionAnimationStart()
{
	CharacterState = ECharacterState::ECS_AnimationPlaying;
}

void AAOSCharacter::TransitionAnimationEnd()
{
	CharacterState = ECharacterState::ECS_Nothing;
}

void AAOSCharacter::ResumeRunning()
{
	RunningButtonPressed();
}

void AAOSCharacter::StopRunning()
{
	RunningButtonReleased();
}

void AAOSCharacter::SetGunRecoil(float Recoil)
{
	GunRecoil = Recoil;
}

UCombatComponent* AAOSCharacter::GetCombatComp() const
{
	return CombatComp;
}

UItemComponent* AAOSCharacter::GetItemComp() const
{
	return ItemComp;
}

void AAOSCharacter::SetWalkingSpeed(EWalkingState State)
{
	if (State == EWalkingState::EWS_Armed)
	{
		CurrentRunningSpeed = ArmedRunningSpeed;
		CurrentWalkingSpeed = ArmedWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeed = ArmedWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = OriginCrouchedSpeed;

	}
	else if (State == EWalkingState::EWS_Slowed)
	{
		CurrentRunningSpeed = SlowedRunningSpeed;
		CurrentWalkingSpeed = SlowedWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeed = SlowedWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = SlowedCroucedSpeed;
	}
	else
	{
		CurrentRunningSpeed = OriginRunningSpeed;
		CurrentWalkingSpeed = OriginWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeed = OriginWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = OriginCrouchedSpeed;
	}
}

void AAOSCharacter::ActivateFreezing(bool IsActivate)
{
	if (IsActivate)
	{
		GetMesh()->bPauseAnims = true;
		CharacterState = ECharacterState::ECS_Freezed;
	}
	else
	{
		GetMesh()->bPauseAnims = false;
		CharacterState = ECharacterState::ECS_Nothing;
	}
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
	PlayerInputComponent->BindAction("Crouch/Dash", IE_Pressed, this, &AAOSCharacter::Crouch_DashButtonPressed);
	PlayerInputComponent->BindAction("Equip/Skill1", IE_Pressed, this, &AAOSCharacter::Equip_Skill1ButtonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AAOSCharacter::AttackButtonPressed);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AAOSCharacter::AttackButtonReleassed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AAOSCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AAOSCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Reload/Skill2", IE_Pressed, this, &AAOSCharacter::Reload_Skill2ButtonPressed);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AAOSCharacter::InventoryKeyPressed);
	PlayerInputComponent->BindAction("WeaponQuickSwap", IE_Pressed, this, &AAOSCharacter::WeaponQuickSwapKeyPressed);
	PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AAOSCharacter::UseItemKeyPressed);
	PlayerInputComponent->BindAction("ItemChange", IE_Pressed, this, &AAOSCharacter::ItemChangeKeyPressed);
	PlayerInputComponent->BindAction("Skill3", IE_Pressed, this, &AAOSCharacter::Skill3ButtonPressed);
}

void AAOSCharacter::SetOverlappingItem()
{
	if (bExistOverlappingItem == false) 
		return;

	FHitResult HitItem;
	TraceItem(HitItem);

	if (HitItem.bBlockingHit)
	{

		AItem* Item = Cast<AItem>(HitItem.Actor);
		if (Item)
		{
			OverlappingItem = Item;
			OverlappingItem->GetWidget()->SetVisibility(true);
		}
		else
		{
			OverlappingItem = nullptr;
		}

		if (OverlappingItemLastFrame)
		{
			if (OverlappingItemLastFrame != OverlappingItem)
			{
				OverlappingItemLastFrame->GetWidget()->SetVisibility(false);
			}
		}

		OverlappingItemLastFrame = OverlappingItem;
	}
}

void AAOSCharacter::TraceItem(FHitResult& HitItem)
{
	FVector2D ViewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	FVector2D CrosshairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector TraceStart = CrosshairWorldPosition;
		FVector TraceEnd = TraceStart + CrosshairWorldDirection * 10000.f;

		GetWorld()->LineTraceSingleByChannel(HitItem, TraceStart, TraceEnd, ECC_FindItem);

		// 적중하지 않았을 경우 타격 지점을 TraceEnd 로 지정
		if (!HitItem.bBlockingHit)
		{
			HitItem.ImpactPoint = TraceEnd;
		}

		DrawDebugLine(GetWorld(), TraceStart+FVector(0.f,0.f,-30.f), HitItem.ImpactPoint, FColor::Blue, false, -1.f, 0U, 2.f);
	}

}

UCameraComponent* AAOSCharacter::GetCamera() const
{
	return Camera;
}

USpringArmComponent* AAOSCharacter::GetSpringArm() const
{
	return SpringArm;
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
	return CharacterState == ECharacterState::ECS_AnimationPlaying;
}

bool AAOSCharacter::GetIsMoving() const
{
	return bIsMoving;
}

bool AAOSCharacter::GetIsAiming() const
{
	return bIsAiming;
}

void AAOSCharacter::DeactivateAiming()
{
	AimButtonReleased();
}

bool AAOSCharacter::GetAttackButtonPressing() const
{
	return bAttackButtonPressing;
}

void AAOSCharacter::SetOverlappingItemCount(int8 Quantity)
{
	if (OverlappingItemCount + Quantity <= 0)
	{
		bExistOverlappingItem = false;
		OverlappingItemCount = 0;
	}
	else
	{
		bExistOverlappingItem = true;
		OverlappingItemCount += Quantity;
	}
}

EWeaponType AAOSCharacter::GetWeaponType() const
{
	return WeaponType;
}

void AAOSCharacter::SetWeaponType(EWeaponType Type)
{
	WeaponType = Type;
}
