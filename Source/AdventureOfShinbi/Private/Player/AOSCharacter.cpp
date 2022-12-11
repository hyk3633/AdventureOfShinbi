

#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "Player/AOSAnimInstance.h"
#include "System/AOSGameModeBase.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyBoss/EnemyBoss.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
#include "Weapons/Projectile.h"
#include "Weapons/ProjectileBullet.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CombatComponent.h"
#include "Components/ItemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "HUD/AOSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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
	SpringArm->bUsePawnControlRotation = true; // 폰의 뷰/컨트롤 회전 값 사용

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	SpringArm->TargetArmLength = 300.f;
	SpringArm->SocketOffset = FVector(0.f, 80.f, 170.f);
	Camera->SetFieldOfView(110.f);
	Camera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));

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

void AAOSCharacter::RestartPlayerCharacter()
{
	CharacterController = Cast<AAOSController>(GetController());
	ItemComp->RestartItemComp();
	CombatComp->RestartCombatComp();

	if (RespawnParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RespawnParticle, GetActorLocation(), GetActorRotation());
	}
	if (RespawnSound)
	{
		UGameplayStatics::PlaySound2D(this, RespawnSound);
	}
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

void AAOSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterController = Cast<AAOSController>(GetController());

	AnimInstance = Cast<UAOSAnimInstance>(GetMesh()->GetAnimInstance());

	//OnTakeAnyDamage.AddDynamic(this, &AAOSCharacter::TakeAnyDamage);
	OnTakePointDamage.AddDynamic(this, &AAOSCharacter::TakePointDamage);
	OnTakeRadialDamage.AddDynamic(this, &AAOSCharacter::TakeRadialDamage);

	CombatComp->PlayerDeathDelegate.AddDynamic(this, &AAOSCharacter::HandlePlayerDeath);
}

void AAOSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetOverlappingItem();
	CameraSaturaion(DeltaTime);
	UpdateControlRotation();
}

void AAOSCharacter::TakeAnyDamage(AActor* DamagedActor, float DamageReceived, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (CharacterState == ECharacterState::ECS_Dead)
		return;
	UE_LOG(LogTemp, Warning, TEXT("any damage %f"), DamageReceived);
	if (CombatComp)
	{
		CombatComp->UpdateHealth(DamageReceived);
	}
}

void AAOSCharacter::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (CharacterState == ECharacterState::ECS_Dead)
		return;

	if (CombatComp)
	{
		AEnemyCharacter* EC = Cast<AEnemyCharacter>(DamageCauser->GetInstigator());
		if (EC)
		{
			const int32 RandInt = FMath::RandRange(1, EC->GetRandRangeValue());
			const float RandValue = EC->GetDefaultValue() * RandInt;
			const float Dmg = (DamageReceived - (CombatComp->Defense / 2)) + RandValue;

			CombatComp->UpdateHealth(Dmg);

			UE_LOG(LogTemp, Warning, TEXT("point damage %f"), Dmg);
		}
	}

	if (CharacterState != ECharacterState::ECS_Nothing)
		return;
	
	AProjectile* Proj = Cast<AProjectile>(DamageCauser);
	if (Proj)
	{
		AProjectileBullet* PB = Cast<AProjectileBullet>(Proj);
		if (PB && FMath::RandRange(0.f, 1.f) <= 0.25f)
		{
			PlayHitReaction(VoicePain, DamageCauser);
		}
		else
		{
			PlayHitReaction(VoicePainHeavy, DamageCauser);
			PlayerKnockBack(DamageCauser, 1000.f);
		}
	}
	else
	{
		AEnemyBoss* EB = Cast<AEnemyBoss>(DamageCauser);
		if (EB)
		{
			if (EB->GetDashAttack())
			{
				PlayHitReaction(VoicePainHeavy, DamageCauser);
			}
			else
			{
				PlayerKnockBack(DamageCauser, 1000.f);
				PlayHitReaction(VoicePain, DamageCauser);
			}
		}
		else if (FMath::RandRange(0.f, 1.f) <= 0.5f)
		{
			PlayerKnockBack(DamageCauser, 450.f);
			PlayHitReaction(VoicePain, DamageCauser);
		}
	}
	
	PlayHitEffect(HitLocation, ShotFromDirection.Rotation());
}

FName AAOSCharacter::DistinguishHitDirection(FVector TargetLocation)
{
	FVector ToTarget = TargetLocation - GetActorLocation();
	ToTarget.Normalize();
	const float Dot = FVector::DotProduct(GetActorForwardVector(), ToTarget);

	FVector Cross = FVector::CrossProduct(ToTarget, GetActorForwardVector());
	Cross.Normalize();
	const float CrossDot = FVector::DotProduct(Cross, GetActorUpVector());

	if (Dot >= 0.3f && Dot < 1.0)
	{
		return FName("F");
	}
	else if (Dot >= -0.3f && Dot < -1.0)
	{
		return FName("B");
	}
	else
	{
		if (CrossDot > 0.f)
		{
			return FName("L");
		}
		else
		{
			return FName("R");
		}
	}
}

void AAOSCharacter::PlayerKnockBack(AActor* DamageCauser, float Power)
{
	FVector Direction = GetActorLocation() - DamageCauser->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();
	LaunchCharacter(Direction * Power, false, false);
	if (CameraShakeHitted)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeHitted);
	}
}

void AAOSCharacter::PlayHitReaction(USoundCue* Voice, AActor* DamageCauser)
{
	if (Voice)
	{
		UGameplayStatics::PlaySound2D(this, Voice);
	}
	CombatComp->PlayHitReactMontage(DistinguishHitDirection(DamageCauser->GetActorLocation()));
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

void AAOSCharacter::InventoryAnimationEnd()
{
	bInventoryAnimationPlaying = false;
}

void AAOSCharacter::ReloadingStart()
{
	CharacterState = ECharacterState::ECS_Reloading;
	if (AnimInstance)
	{
		AnimInstance->ActivateReloading();
	}
}

void AAOSCharacter::ReloadingEnd()
{
	CharacterState = ECharacterState::ECS_Nothing;
	if (AnimInstance)
	{
		AnimInstance->DeactivateReloading();
	}
}

void AAOSCharacter::PlayerRespawn()
{
	GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->RespawnPlayer();
}

void AAOSCharacter::CameraSaturaion(float DeltaTime)
{
	if (bCameraSaturationOn)
	{
		InitialSaturation = FMath::FInterpTo(InitialSaturation, 0.f, DeltaTime, 1.f);
		Camera->PostProcessSettings.ColorSaturation = FVector4(InitialSaturation, InitialSaturation, InitialSaturation);
	}
}

void AAOSCharacter::TakeRadialDamage(AActor* DamagedActor, float DamageReceived, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (CharacterState == ECharacterState::ECS_Dead)
		return;

	AEnemyCharacter* EC = Cast<AEnemyCharacter>(DamageCauser->GetInstigator());
	if (EC)
	{
		const int32 RandInt = FMath::RandRange(1, EC->GetRandRangeValue());
		const float RandValue = EC->GetDefaultValue() * RandInt;
		const float Dmg = (DamageReceived - (CombatComp->Defense / 2)) + RandValue;

		CombatComp->UpdateHealth(Dmg);

		UE_LOG(LogTemp, Warning, TEXT("radial %f"), Dmg);
	}

	PlayHitReaction(VoicePainHeavy, DamageCauser);
	PlayerKnockBack(DamageCauser, 1000.f);
}

void AAOSCharacter::HandlePlayerDeath()
{
	if (FreezingSign)
	{
		FreezingSign->Destruct();
		FreezingSign = nullptr;
	}
	if (SlowSign)
	{
		SlowSign->Destruct();
		SlowSign = nullptr;
	}

	bCameraSaturationOn = true;
	SetCharacterState(ECharacterState::ECS_Dead);
	GetMovementComponent()->StopMovementImmediately();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(PlayerRespawnTimer, this, &AAOSCharacter::PlayerRespawn, PlayerRespawnTime);
}

void AAOSCharacter::MoveForward(float Value)
{
	if (CharacterState != ECharacterState::ECS_Nothing && CharacterState != ECharacterState::ECS_Reloading  || bDashing)
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
	if (CharacterState != ECharacterState::ECS_Nothing && CharacterState != ECharacterState::ECS_Reloading || bDashing)
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
	if (CharacterState == ECharacterState::ECS_Freezed || CharacterState == ECharacterState::ECS_Dead)
		return;

	AddControllerPitchInput(Value);
}

void AAOSCharacter::Turn(float Value)
{
	if (CharacterState == ECharacterState::ECS_Freezed || CharacterState == ECharacterState::ECS_Dead)
		return;

	AddControllerYawInput(Value);
}

void AAOSCharacter::Jump()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bDashing)
		return;

	MakeNoise(1.f, GetInstigator(), GetActorLocation());

	if (VoiceJump)
	{
		UGameplayStatics::PlaySoundAtLocation(this, VoiceJump, GetActorLocation());
	}

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
	MakeNoise(1.f, GetInstigator(), GetActorLocation());
}

void AAOSCharacter::Crouch_DashButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if(CombatComp->GetEquippedWeapon() == nullptr)
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
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	DSecretInteraction.Broadcast();

	if (bOverlappedLTV)
	{
		DLevelTransition.ExecuteIfBound();
	}
	else if (OverlappingItem)
	{
		OverlappingItem->PlayGainEffect();
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
	bAttackButtonPressing = true;

	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr || CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	bAbleAttack = true;

	DAttackButtonPressed.ExecuteIfBound();

	if (CombatComp->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Glaive || CombatComp->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
	{
		CombatComp->MeleeAttack();
	}
	else
	{
		Fire();
	}
}

void AAOSCharacter::AttackButtonReleassed()
{
	bAttackButtonPressing = false;
	bAbleAttack = false;
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
	bAbleFire = false;
	ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->EquippedWeapon);
	GetWorldTimerManager().SetTimer(FireTimer, this, &AAOSCharacter::FireReturn, RW->GetFireRate());
}

void AAOSCharacter::FireReturn()
{
	bAbleFire = true;
	ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->EquippedWeapon);
	if (RW->GetAutomaticFire() && bAbleAttack)
	{
		Fire();
	}
}

void AAOSCharacter::PlayDashMotage()
{
	if (DashMontage)
	{
		MakeNoise(1.f, GetInstigator(), GetActorLocation());
		AnimInstance->Montage_Play(DashMontage);
	}
}

void AAOSCharacter::OnDashMontageEnded()
{
	bDashing = false;
}

void AAOSCharacter::AimButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	if (CombatComp->EquippedWeapon)
	{
		DAimButtonPressed.ExecuteIfBound(true);

		ARangedWeapon* RW = Cast<ARangedWeapon>(CombatComp->EquippedWeapon);
		if (RW)
		{
			bIsAiming = true;

			SpringArm->TargetArmLength = 200.f;
			SpringArm->SocketOffset = FVector(0.f, 50.f, 80.f);
			Camera->SetFieldOfView(110.f);
			Camera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		}
		else if (CombatComp->GetEquippedWeapon()->GetWeaponType() == EWeaponType::EWT_ShinbiSword)
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

	if (CombatComp->GetEquippedWeapon())
	{
		bIsAiming = false;

		SetView(CombatComp->GetEquippedWeapon()->GetWeaponType());
	}
}

void AAOSCharacter::Reload_Skill2ButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	CombatComp->WeaponSkill2();
}

void AAOSCharacter::InventoryKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing)
		return;

	if (CharacterController && bInventoryAnimationPlaying == false)
	{
		bIsInventoryOn = bIsInventoryOn ? false : true;
		bInventoryAnimationPlaying = true;
		CharacterController->HUDInventoryOn(bIsInventoryOn);
		GetWorldTimerManager().SetTimer(InventoryAnimationTimer, this, &AAOSCharacter::InventoryAnimationEnd, 0.7f);
	}
}

void AAOSCharacter::WeaponQuickSwapKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	if (CombatComp)
	{
		CombatComp->WeaponQuickSwap();
	}
}

void AAOSCharacter::UseItemKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	if (ItemComp)
	{
		ItemComp->UseActivatedQuickSlotItem();
	}
}

void AAOSCharacter::ItemChangeKeyPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	if (CharacterController)
	{
		CharacterController->ItemChange();
	}
}

void AAOSCharacter::Skill3ButtonPressed()
{
	if (CharacterState != ECharacterState::ECS_Nothing || bIsInventoryOn || bInventoryAnimationPlaying)
		return;

	if (CombatComp->EquippedWeapon)
	{
		CombatComp->WeaponSkill3();
	}
}

void AAOSCharacter::MouseWheelButtonPressed()
{
	if (CombatComp->EquippedWeapon == nullptr)
		return;
	
	bCameraLockOn = bCameraLockOn ? false : true;

	if (bCameraLockOn)
	{
		SetLockOnTarget();
	}
	else
	{
		LockOnTarget = nullptr;
		if (LockOnParticleComp)
		{
			LockOnParticleComp->DestroyComponent();
		}
	}
}

void AAOSCharacter::SetLockOnTarget()
{
	LockOnTarget = Cast<AEnemyCharacter>(FindTarget());
	if (LockOnTarget)
	{
		const float CharacterHeight = LockOnTarget->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 50.f;
		LockOnParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			LockOnParticle,
			LockOnTarget->GetMesh(),
			NAME_None,
			FVector(0.f, 0.f, CharacterHeight),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false,
			EPSCPoolMethod::None,
			true
		);
	}
	else
	{
		bCameraLockOn = false;
	}
}

void AAOSCharacter::UpdateControlRotation()
{
	if (CharacterController == nullptr || CombatComp->EquippedWeapon == nullptr)
	{
		bCameraLockOn = false;
		LockOnTarget = nullptr;
		if (LockOnParticleComp)
		{
			LockOnParticleComp->DestroyComponent();
		}
		return;
	}

	if (bCameraLockOn)
	{
		if (LockOnTarget->GetIsDead())
		{
			LockOnTarget = nullptr;
			if (LockOnParticleComp)
			{
				LockOnParticleComp->DestroyComponent();
			}
			SetLockOnTarget();
		}
		else
		{
			const FRotator LookRotation = CalculateRotation(LockOnTarget);
			CharacterController->SetControlRotation(LookRotation);
		}
	}
}

AActor* AAOSCharacter::FindTarget()
{
	TArray<AActor*> Targets;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Targets);
	
	if(Targets.Num() == 0)
		return nullptr;

	float MinDistance = 2000.f;
	AActor* NearestTarget = nullptr;
	for (AActor* Target : Targets)
	{
		if (DistinguishHitDirection(Target->GetActorLocation()) != FName("F"))
			continue;

		const float Distance = GetDistanceTo(Target);
		if (Distance <= MinDistance)
		{
			MinDistance = Distance;
			NearestTarget = Target;
		}
	}

	return NearestTarget;
}

FRotator AAOSCharacter::CalculateRotation(AActor* Target)
{
	if (IsValid(Target))
	{
		const FRotator ControllerRotator = CharacterController->GetControlRotation();
		const FVector PlayerLocation = GetActorLocation();
		const FVector TargetLocation = Target->GetActorLocation();

		const FRotator LookAtTarget = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);
		const FRotator InterpRotator = FMath::RInterpTo(ControllerRotator, LookAtTarget, GetWorld()->GetDeltaSeconds(), 30.f);

		return InterpRotator;
	}

	return FRotator::ZeroRotator;
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
		if (SlowSignClass)
		{
			SlowSign = CreateWidget<UUserWidget>(GetWorld(), SlowSignClass);
			if (SlowSign)
			{
				SlowSign->AddToViewport();
			}
		}
	}
	else
	{
		CurrentRunningSpeed = OriginRunningSpeed;
		CurrentWalkingSpeed = OriginWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeed = OriginWalkingSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = OriginCrouchedSpeed;
		if (SlowSign)
		{
			SlowSign->Destruct();
			SlowSign = nullptr;
		}
	}
}

void AAOSCharacter::ActivateFreezing(bool IsActivate)
{
	if (IsActivate)
	{
		GetMesh()->bPauseAnims = true;
		CharacterState = ECharacterState::ECS_Freezed;
		if (FreezingSignClass)
		{
			FreezingSign = CreateWidget<UUserWidget>(GetWorld(), FreezingSignClass);
			if (FreezingSign)
			{
				FreezingSign->AddToViewport();
			}
		}
	}
	else
	{
		GetMesh()->bPauseAnims = false;
		CharacterState = ECharacterState::ECS_Nothing;
		if (FreezingSign)
		{
			FreezingSign->Destruct();
			FreezingSign = nullptr;
		}
	}
}

void AAOSCharacter::SetView(EWeaponType Type)
{
	if (Type == EWeaponType::EWT_AK47 || Type == EWeaponType::EWT_Revenent || Type == EWeaponType::EWT_Wraith)
	{
		SpringArm->TargetArmLength = 200.f;
		SpringArm->SocketOffset = FVector(0.f, 80.f, 100.f);
		Camera->SetFieldOfView(110.f);
		Camera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	}
	else
	{
		SpringArm->TargetArmLength = 300.f;
		SpringArm->SocketOffset = FVector(0.f, 80.f, 170.f);
		Camera->SetFieldOfView(110.f);
		Camera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	}
}

void AAOSCharacter::ActivateWeaponControlMode()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AAOSCharacter::DeactivateWeaponControlMode()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AAOSCharacter::SetOverlappedLTV(bool bIsOverlap)
{
	bOverlappedLTV = bIsOverlap;
}

void AAOSCharacter::SetCharacterController()
{
	CharacterController = Cast<AAOSController>(GetController());
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
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AAOSCharacter::MouseWheelButtonPressed);
}

void AAOSCharacter::SetOverlappingItem()
{
	if (bExistOverlappingItem == false)
	{
		if (OverlappingItemLastFrame)
		{
			OverlappingItemLastFrame->GetWidget()->SetVisibility(false);
			OverlappingItemLastFrame = nullptr;
		}
		return;
	}

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
		if (OverlappingItemLastFrame)
		{
			if (OverlappingItemLastFrame != OverlappingItem)
			{
				OverlappingItemLastFrame->GetWidget()->SetVisibility(false);
			}
		}
		OverlappingItemLastFrame = OverlappingItem;
	}
	else
	{
		if (OverlappingItem)
		{
			OverlappingItem->GetWidget()->SetVisibility(false);
			OverlappingItem = nullptr;
			OverlappingItemLastFrame = nullptr;
		}
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
		FVector TraceStart = CrosshairWorldPosition - FVector(0.f,0.f,50.f);
		FVector TraceEnd = TraceStart + CrosshairWorldDirection * 10000.f;

		UCollisionProfile* Profile = UCollisionProfile::Get();
		ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_FindItem);

		UKismetSystemLibrary::CapsuleTraceSingle
		(
			this,
			TraceStart,
			TraceEnd,
			35.f,
			20.f,
			TraceType,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			HitItem,
			true
		);
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

UAOSAnimInstance* AAOSCharacter::GetAnimInst() const
{
	return AnimInstance;
}

void AAOSCharacter::SetCharacterState(ECharacterState State)
{
	CharacterState = State;
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

void AAOSCharacter::CallAttackFunction()
{
	AttackButtonPressed();
}

void AAOSCharacter::SetAbleAttackFalse()
{
	bAbleAttack = false;
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