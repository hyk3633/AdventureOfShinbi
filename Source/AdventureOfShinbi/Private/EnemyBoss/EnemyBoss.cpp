

#include "EnemyBoss/EnemyBoss.h"
#include "EnemyBoss/BossAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CombatComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

AEnemyBoss::AEnemyBoss()
{
	PerceptionComp->Deactivate();

	DashAndWallBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DashAndWallBox"));
	DashAndWallBox->SetupAttachment(RootComponent);
	DashAndWallBox->SetCollisionObjectType(ECC_EnemyWeaponTrace);
	DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxTraceSize = FVector(30.f, 30.f, 30.f);

	StrafingAnimRate = 0.6f;
	ChaseAnimRate = 1.f;
	PatrolSpeed = 400.f;
	ChaseSpeed = 700.f;
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	Target = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Target)
	{
		Target->DAttackButtonPressed.BindUObject(this, &AEnemyBoss::DetectAttack);
	}

	BossController = Cast<ABossAIController>(GetController());
	if (BossController)
	{
		BossController->SetTarget(Target);
	}

	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyBoss::BlizzardMontageEnd);

	DashAndWallBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::OnDashBoxOverlap);

	GetWorldTimerManager().SetTimer(FreezingCoolTimer, this, &AEnemyBoss::FreezingCoolTimeEnd, 15.f);
	GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, 15.f);
	GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, 15.f);
}

void AEnemyBoss::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);

	RotateToTarget(DeltaTime);

	DoStrafing();

	BlizzardDotDamage(DeltaTime);

	Weapon1BoxTrace();

	BurstShockWave();
}

void AEnemyBoss::RotateToTarget(float DeltaTime)
{
	if (
		bDeath == false &&
		bFreezingAttack == false &&
		AiInfo.bIsPlayerDead == false
		)
	{
		FRotator Rotation = GetActorRotation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		LookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

		float RotateRate = bIsAttacking ? 10.f : 20.f;

		Rotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotateRate);
		SetActorRotation(Rotation);
	}
}

void AEnemyBoss::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	HandleHealthChange(DamageReceived);

	if (GetHealthPercentage() <= 0.6f)
	{
		bPhase2 = true;
		DashDelayTime = 0.5f;
		MaxDashCount = 6;
	}
}

void AEnemyBoss::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsAttacking)
		return;

	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		AiInfo.bTargetInAttackRange = true;
		if (BossController)
		{
			BossController->UpdateAiInfo();
		}
	}
}

void AEnemyBoss::OnDashBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha == nullptr) 
		return;

	const FVector TargetLocation = Cha->GetActorLocation();
	const float Dist = FVector::Dist(DashStartLocation, TargetLocation);

	FVector ToTarget = GetActorLocation() - TargetLocation;
	ToTarget.Normalize();
	float Dot = FVector::DotProduct(Cha->GetActorForwardVector(), ToTarget);

	FVector Cross = FVector::CrossProduct(ToTarget, Cha->GetActorForwardVector());
	Cross.Normalize();
	float CrossDot = FVector::DotProduct(Cross, Cha->GetActorUpVector());
	
	const float RandFloat = FMath::RandRange(30.f, 60.f);
	float RotateValue = 0.f;
	if (Dot < 0)
	{
		RotateValue = CrossDot < 0 ? RandFloat : RandFloat * -1.f;
	}
	else
	{
		RotateValue = CrossDot < 0 ? RandFloat + 90.f : (RandFloat + 90.f) * -1.f;
	}
	FVector Forward = GetActorForwardVector();
	const FVector LaunchDirection = Forward.RotateAngleAxis(RotateValue, FVector(0, 0, 1));

	Cha->LaunchCharacter(LaunchDirection * Dist * 5.f, false, true);
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeDash);

	if (DashHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashHitSound, Cha->GetActorLocation());
	}

	UGameplayStatics::ApplyPointDamage(Cha, Damage, LaunchDirection, SweepResult, GetController(), this, UDamageType::StaticClass());

	SetBoxState(EBoxState::EBS_Disabled);
}

void AEnemyBoss::DetectAttack()
{
	if (bEvadeSkillCoolTimeEnd == false)
		return;

	EWeaponType TargetWeaponType = Target->GetCombatComp()->GetEquippedWeapon()->GetWeaponType();

	if (bPhase2 && TargetWeaponType == EWeaponType::EWT_Gun && GetDistanceTo(Target) > 700.f)
	{
		EvadeSkillNum = FMath::RandRange(1, 2);
	}
	else
	{
		EvadeSkillNum = FMath::RandRange(0, 1);
	}

	bAbleEvadeSkill = true;
	if (BossController)
	{
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
}

bool AEnemyBoss::Weapon1BoxTrace()
{
	const bool bHit = Super::Weapon1BoxTrace();

	if (bHit)
	{
		if (EnemyAnim->Montage_GetIsStopped(FreezingMontage) == false)
		{
			FreezingActivate();
		}
	}

	return bHit;
}

void AEnemyBoss::DoStrafing()
{
	const bool StrafingCondition = CheckStrafingCondition();
	if (StrafingCondition)
	{
		SetStrafingValue();
		GetCharacterMovement()->MaxWalkSpeed = StrafingSpeed;
	}
	else
	{
		StrafingValue = 0.f;
		GetCharacterMovement()->MaxWalkSpeed = bPhase2 ? ChaseSpeed : PatrolSpeed;
	}
}

bool AEnemyBoss::CheckStrafingCondition()
{
	return bDeath == false &&
		bStrafing &&
		bIsAttacking == false &&
		AiInfo.bStunned == false;
}

void AEnemyBoss::ChangeStrafingDirection()
{
	const int32 RandDir = FMath::RandRange(0, 1);

	switch (RandDir)
	{
	case 0:
		StrafingDir = EStrafingDirection::ESD_RDiagonal;
		break;
	case 1:
		StrafingDir = EStrafingDirection::ESD_LDiagonal;
		break;
	}

	StrafingTime = 3.f + FMath::RandRange(1.f, 5.f);

	GetWorldTimerManager().SetTimer(StrafingTimer, this, &AEnemyBoss::ChangeStrafingDirection, StrafingTime);
}

void AEnemyBoss::RangedAttackCoolTimeEnd()
{
	bRangedAttackCoolTimeEnd = true;
	if (BossController)
	{
		BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
	}
}

void AEnemyBoss::EvadeSkillCoolTimeEnd()
{
	bEvadeSkillCoolTimeEnd = true;
}

void AEnemyBoss::SetFlying()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void AEnemyBoss::PlayDeathMontage()
{
	Super::PlayDeathMontage();

	UParticleSystemComponent* SwordParticle = Cast<UParticleSystemComponent>(GetMesh()->GetChildComponent(0));
	if (SwordParticle)
	{
		SwordParticle->Deactivate();
	}
}

void AEnemyBoss::DeathMontageEnded()
{
	GetMesh()->bPauseAnims = true;
}

void AEnemyBoss::Attack()
{
	if (bDeath)
		return;

	bIsAttacking = true;
	if (BossController)
	{
		BossController->UpdateAiInfo();
	}

	if (bFreezingCoolTimeEnd && FMath::RandRange(0.f,1.f) < 0.3f)
	{
		Freezing();
	}
	else
	{
		PlayAttackMontage();
	}
}

void AEnemyBoss::RangedAttack()
{
	if (bDeath)
		return;

	bIsAttacking = true;
	if (BossController)
	{
		BossController->UpdateAiInfo();
	}

	int8 RandNum = 0;
	const float DistToTarget = GetDistanceTo(AiInfo.TargetPlayer);
	if (DistToTarget > 800.f)
	{
		RandNum = FMath::RandRange(1, bPhase2 ? 3 : 2);
	}
	else if (DistToTarget > 600.f && DistToTarget <= 800.f)
	{
		RandNum = FMath::RandRange(0, bPhase2 ? 3 : 2);
	}
	else
	{
		RandNum = 0;
	}

	switch (RandNum)
	{
	case 0:
		Blizzard();
		break;
	case 1:
		EmitSwordAura();
		break;
	case 2:
		Dash();
		break;
	case 3:
		IcicleAttack();
		break;
	}
}

void AEnemyBoss::EvadeSkill()
{
	if (bDeath)
		return;

	bIsAttacking = true;
	if (BossController)
	{
		BossController->UpdateAiInfo();
	}

	if (EvadeSkillNum == 0)
	{
		Evade();
	}
	else if (EvadeSkillNum == 1)
	{
		BackAttack();
	}
	else
	{
		CreateIceWall();
	}
}

void AEnemyBoss::PlayAttackMontage()
{
	if (EnemyAnim == nullptr) 
		return;

	const int8 RandNum = FMath::RandRange(0, bPhase2 ? 2 : 1);
	
	if (RandNum == 0)
	{
		PlayAttack1Montage();
	}
	else if (RandNum == 1)
	{
		PlayAttack2Montage();
	}
	else
	{
		GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
		PlayAttack3Montage();
	}
}

void AEnemyBoss::PlayAttack1Montage()
{
	if (AttackMontage == nullptr) 
		return;
	EnemyAnim->Montage_Play(AttackMontage);
}

void AEnemyBoss::PlayAttack2Montage()
{
	if (Attack2Montage == nullptr) 
		return;
	EnemyAnim->Montage_Play(Attack2Montage);
}

void AEnemyBoss::PlayAttack3Montage()
{
	if (Attack3Montage == nullptr) 
		return;
	EnemyAnim->Montage_Play(Attack3Montage);
}

void AEnemyBoss::MoveWhileAttack()
{
	LaunchCharacter(GetActorForwardVector() * 1000.f, false, true);
}

void AEnemyBoss::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage || Montage == Attack2Montage || Montage == Attack3Montage)
	{
		bIsAttacking = false;
		if (GetAttackRange()->IsOverlappingActor(AiInfo.TargetPlayer) == false)
			AiInfo.bTargetInAttackRange = false;
		if (BossController)
		{
			BossController->UpdateAiInfo();
		}
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		OnAttackEnd.Broadcast();
	}
}

void AEnemyBoss::StopAttackMontage()
{
	if (bActivateWeaponTrace1)
	{
		bActivateWeaponTrace1 = false;

		if (EnemyAnim && AIController)
		{
			EnemyAnim->Montage_JumpToSection(FName("CancelCombo"));
		}
	}
}

void AEnemyBoss::Freezing()
{
	bFreezingAttack = true;
	PlayFreezingMontage();
}

void AEnemyBoss::FreezingActivate()
{
	if (Target == nullptr)
	{
		FreezingMontageEnded();
		return;
	}

	Target->ActivateFreezing(true);

	if (TargetFreezingParticle)
	{
		FTransform ParticleTransform;
		FVector TargetLocation = Target->GetActorLocation();
		TargetLocation.Z = -40.f;
		ParticleTransform.SetLocation(TargetLocation);
		ParticleTransform.SetRotation(Target->GetActorRotation().Quaternion());

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetFreezingParticle, ParticleTransform, true);
	}

	if (FreezingSound)
	{
		UGameplayStatics::PlaySound2D(this, FreezingSound);
	}

	GetWorldTimerManager().SetTimer(FreezingDurationTimer, this, &AEnemyBoss::FreezingDurationEnd, FreezingDurationTime);
}

void AEnemyBoss::PlayFreezingMontage()
{
	if (FreezingMontage == nullptr)
		return;

	EnemyAnim->Montage_Play(FreezingMontage);
}

void AEnemyBoss::FreezingDurationEnd()
{
	Target->ActivateFreezing(false);
}

void AEnemyBoss::FreezingMontageEnded()
{
	bIsAttacking = false;
	bFreezingAttack = false;
	bFreezingCoolTimeEnd = false;
	AiInfo.bTargetInAttackRange = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
	}
	GetWorldTimerManager().SetTimer(FreezingCoolTimer, this, &AEnemyBoss::FreezingCoolTimeEnd, FreezingCoolTime);
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::FreezingCoolTimeEnd()
{
	bFreezingCoolTimeEnd = true;
}

void AEnemyBoss::Blizzard()
{
	PlayBlizzardMontage();
}

void AEnemyBoss::CheckTargetInBlizzardRange()
{
	TArray<AActor*> InRangeTarget;
	UGameplayStatics::GetAllActorsOfClass(this, AAOSCharacter::StaticClass(), InRangeTarget);
	if (InRangeTarget.Num() > 0)
	{
		if (InRangeTarget[0] == Target)
		{
			if(GetDistanceTo(Target) <= 1000.f)
				ApplyBlizzardDebuff();
		}
	}
}

void AEnemyBoss::PlayBlizzardMontage()
{
	if (BlizzardMontage == nullptr)
		return;
	EnemyAnim->Montage_Play(BlizzardMontage);
}

void AEnemyBoss::ApplyBlizzardDebuff()
{
	bBlizzardDebuffOn = true;

	Target->SetWalkingSpeed(EWalkingState::EWS_Slowed);

	GetWorldTimerManager().SetTimer(BlizzardDebuffTimer, this, &AEnemyBoss::BlizzardDebuffEnd, BlizzardDebuffTime);
}

void AEnemyBoss::BlizzardDotDamage(float DeltaTime)
{
	if (bBlizzardDebuffOn == false)
		return;
	UGameplayStatics::ApplyDamage(Target, DeltaTime * 4.f, GetController(), this, UDamageType::StaticClass());
}

void AEnemyBoss::BlizzardDebuffEnd()
{
	bBlizzardDebuffOn = false;

	Target->GetCombatComp()->GetEquippedWeapon() == nullptr ? 
		Target->SetWalkingSpeed(EWalkingState::EWS_UnArmed) : Target->SetWalkingSpeed(EWalkingState::EWS_Armed);
}

void AEnemyBoss::BlizzardMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BlizzardMontage)
	{
		bIsAttacking = false;
		bRangedAttackCoolTimeEnd = false;
		if (BossController)
		{
			BossController->UpdateAiInfo();
			BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
		}
		GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, RangedAttackCoolTime);
		OnAttackEnd.Broadcast();
	}
}

void AEnemyBoss::Dash()
{
	DashCount++;

	SetBoxState(EBoxState::EBS_Dash);

	DashStartLocation = GetActorLocation();

	PlayDashMontage();
}

void AEnemyBoss::PlayDashMontage()
{
	if (DashMontage == nullptr) 
		return;
	bDashAttack = true;
	EnemyAnim->Montage_Play(DashMontage);
}

void AEnemyBoss::DashLaunch()
{
	float dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

	LaunchCharacter(GetActorForwardVector() * (dist * 8), false, true);
}

void AEnemyBoss::DashMontageEnd()
{
	if (DashCount < MaxDashCount)
	{
		Dash();
	}
	else
	{
		SetBoxState(EBoxState::EBS_Disabled);
		bIsAttacking = false;
		bRangedAttackCoolTimeEnd = false;
		bDashAttack = false;
		DashCount = 0;
		if (BossController)
		{
			BossController->UpdateAiInfo();
			BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
		}
		GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, RangedAttackCoolTime);
		OnAttackEnd.Broadcast();
	}
}

void AEnemyBoss::SetBoxState(EBoxState State)
{
	switch (State)
	{
	case EBoxState::EBS_Disabled:
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DashAndWallBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		break;
	case EBoxState::EBS_Dash:
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DashAndWallBox->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
		DashAndWallBox->SetBoxExtent(FVector(50.f, 80.f, 150.f));
		DashAndWallBox->SetRelativeLocation(FVector(180.f, 0.f, 60.f));
		break;
	case EBoxState::EBS_Wall:
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DashAndWallBox->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
		DashAndWallBox->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Block);
		DashAndWallBox->SetBoxExtent(FVector(100.f, 270.f, 220.f));

		const FVector WallLocation = GetActorLocation()+ (GetActorForwardVector() * 250.f) + FVector(0.f,0.f,70.f);
		DashAndWallBox->SetWorldLocation(WallLocation);
		DashAndWallBox->SetWorldRotation(GetActorRotation());
		DashAndWallBox->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		break;
	}
}

void AEnemyBoss::EmitSwordAura()
{
	PlayFireMontage();
}

void AEnemyBoss::FireSwordAura()
{
	const USkeletalMeshSocket* EmitSocket = GetMesh()->GetSocketByName("Sword_Mid");
	if (EmitSocket == nullptr) return;
	const FTransform SocketTransform = EmitSocket->GetSocketTransform(GetMesh());

	FVector ToTarget = Target->GetActorLocation() - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();

	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = this;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		}
	}
}

void AEnemyBoss::PlayFireMontage()
{
	if (FireMontage == nullptr)
		return;

	EnemyAnim->Montage_Play(FireMontage);
}

void AEnemyBoss::OnFireMontageEnded()
{
	bIsAttacking = false;
	bRangedAttackCoolTimeEnd = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
	}
	GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, RangedAttackCoolTime);
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::IcicleAttack()
{
	PlayIcicleAttackMontage();
}

void AEnemyBoss::RisingIcicle()
{
	if (IcicleParticle)
	{
		FTransform ParticleTransform;
		ParticleTransform.SetLocation(Target->GetActorLocation() + FVector(0.f, 120.f, 0.f));
		ParticleTransform.SetRotation(FRotator(0.f, 120.f, 0.f).Quaternion());
		ParticleTransform.SetScale3D(FVector(5.f, 5.f, 5.f));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IcicleParticle, ParticleTransform);
		if (IcicleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, IcicleSound, ParticleTransform.GetLocation());
		}

		UCollisionProfile* Profile = UCollisionProfile::Get();
		ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_EnemyWeaponTrace);
		FHitResult SphereHit;
		UKismetSystemLibrary::SphereTraceSingle
		(
			this,
			IcicleLocation + FVector(0.f, 0.f, 10.f),
			IcicleLocation + FVector(0.f, 0.f, 10.f),
			100.f,
			TraceType,
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::None,
			SphereHit,
			true
		);

		if (SphereHit.bBlockingHit)
		{
			if (Target == SphereHit.GetActor())
			{
				UGameplayStatics::ApplyPointDamage(Target, 70.f, SphereHit.ImpactPoint, SphereHit, GetController(), this, UDamageType::StaticClass());
				Target->LaunchCharacter(Target->GetActorUpVector() * 500.f, true, false);
			}
		}
	}
}

void AEnemyBoss::SetIcicleLocation()
{
	IcicleLocation = Target->GetActorLocation();
}

void AEnemyBoss::BurstShockWave()
{
	if (bActivateBurst == false)
		return;

	FHitResult WeaponHitResult;
	GetBoxTraceHitResult(WeaponHitResult, WeaponTraceStartSocketName, WeaponTraceEndSocketName);

	if (WeaponHitResult.bBlockingHit)
	{
		TArray<AActor*> Ignore;
		UGameplayStatics::ApplyRadialDamage(
			this,
			BurstDamage,
			WeaponHitResult.ImpactPoint,
			100.f,
			UDamageType::StaticClass(),
			Ignore,
			this,
			GetController()
		);

		bActivateBurst = false;
	}
}

void AEnemyBoss::PlayIcicleAttackMontage()
{
	if (IcicleAttackMontage == nullptr)
		return;
	EnemyAnim->Montage_Play(IcicleAttackMontage);
}

void AEnemyBoss::IcicleAttackMontageEnded()
{
	bIsAttacking = false;
	bRangedAttackCoolTimeEnd = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
	}
	GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, RangedAttackCoolTime);
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::ActivateBurst()
{
	bActivateBurst = true;
}

void AEnemyBoss::DeactivateBurst()
{
	bActivateBurst = false;
}

void AEnemyBoss::CreateIceWall()
{
	SetBoxState(EBoxState::EBS_Wall);

	if (IceWallParticle)
	{
		FTransform ParticleTransform;
		ParticleTransform.SetLocation(GetActorLocation() + FVector(50.f, 0.f, -50.f));
		ParticleTransform.SetRotation(GetActorRotation().Quaternion());
		ParticleTransform.SetScale3D(FVector(6.f, 6.f, 6.f));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IceWallParticle, ParticleTransform);

		if (IcicleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, IcicleSound, ParticleTransform.GetLocation());
		}
	}

	GetWorldTimerManager().SetTimer(IceWallAttackDisappearTimer, this, &AEnemyBoss::IcaWallDisappearAndCheck, IceWallAttackDisappearTime);
}

void AEnemyBoss::IcaWallDisappearAndCheck()
{
	GetMesh()->SetVisibility(false);

	int8 RandNum = FMath::RandRange(0, 2);
	FName Dir = RandNum == 0 ? FName("L") : RandNum == 1 ? FName("R") : FName("B");

	bool bSafe = CheckObstacle(Target, Dir, 250.f);
	if (bSafe == false)
	{
		BackAttackMontageEnd(false);
		return;
	}
	SetRotationToAppear(Dir);

	bEvadeSkillCoolTimeEnd = false;
	PlayIceWallAttackMontage();
	GetWorldTimerManager().SetTimer(IceWallAttackAppearTimer, this, &AEnemyBoss::AppearFromLRB, IceWallAttackAppearTime);
}

void AEnemyBoss::SetRotationToAppear(FName Direction)
{
	if (Direction == FName("L"))
	{
		RotationToAppear = Target->GetActorRightVector().Rotation();
	}
	else if (Direction == FName("R"))
	{
		RotationToAppear = Target->GetActorRightVector().Rotation() * -1.f;
	}
	else
	{
		RotationToAppear = Target->GetActorRotation();
	}
}

void AEnemyBoss::PlayIceWallAttackMontage()
{
	if (IceWallAttackMontage == nullptr)
		return;

	const int8 RandNum = FMath::RandRange(0, 1);

	EnemyAnim->Montage_Play(IceWallAttackMontage);
	EnemyAnim->Montage_JumpToSection(FName(*FString::FromInt(RandNum)));
}

void AEnemyBoss::IceWallAttackMontageEnd(bool IsSuccess)
{
	bIsAttacking = false;
	bAbleEvadeSkill = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
	SetBoxState(EBoxState::EBS_Disabled);
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, EvadeSkillCoolTime);
	}
	else
	{
		EvadeSkillCoolTimeEnd();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::BackAttack()
{
	if (Target == nullptr)
		return;

	UParticleSystemComponent* SwordParticle = Cast<UParticleSystemComponent>(GetMesh()->GetChildComponent(0));
	if (SwordParticle)
	{
		SwordParticle->Deactivate();
	}

	if (DecoyParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DecoyParticle, GetActorLocation(), GetActorRotation(), true);
	}
	if (DecoyDestroyParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DecoyDestroyParticle, GetActorLocation(), GetActorRotation(), true);
	}
	if (DecoyDestroySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DecoyDestroySound, GetActorLocation());
	}
	if (DisappearSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DisappearSound, GetActorLocation());
	}
	GetMesh()->SetVisibility(false);

	GetWorldTimerManager().SetTimer(DisappearTimer, this, &AEnemyBoss::BackAttackDisappearAndCheck, DisappearTime);
}

void AEnemyBoss::BackAttackDisappearAndCheck()
{
	bool bSafe = CheckObstacle(Target, FName("B"), 250.f);
	if (bSafe == false)
	{
		BackAttackMontageEnd(false);
		return;
	}

	RotationToAppear = GetActorRotation();
	RotationToAppear.Yaw = Target->GetActorRotation().Yaw;

	bEvadeSkillCoolTimeEnd = false;
	PlayBackAttackMontage();
	GetWorldTimerManager().SetTimer(BackAttackAppearTimer, this, &AEnemyBoss::AppearFromLRB, BackAttackAppearTime);
}

bool AEnemyBoss::CheckObstacle(AActor* CenterActor, FName Direction, float Offset)
{
	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	FHitResult BoxHit;

	FVector BoxTraceStart = CenterActor->GetActorLocation();
	FVector BoxExtent;

	if (Direction == FName("L"))
	{
		BoxTraceStart = BoxTraceStart + (-CenterActor->GetActorRightVector() * Offset) + FVector(0.f, 0.f, -30.f);
		BoxExtent = FVector(70.f, 80.f, 30.f);
	}
	else if (Direction == FName("R"))
	{
		BoxTraceStart = BoxTraceStart + (CenterActor->GetActorRightVector() * Offset) + FVector(0.f, 0.f, -30.f);
		BoxExtent = FVector(70.f, 80.f, 30.f);
	}
	else if (Direction == FName("B"))
	{
		BoxTraceStart = BoxTraceStart + (-CenterActor->GetActorForwardVector() * Offset) + FVector(0.f, 0.f, -30.f);
		BoxExtent = FVector(100.f, 70.f, 30.f);
	}
	else
	{
		return false;
	}

	const FVector BoxTraceEnd = BoxTraceStart + FVector(0.f, 0.f, 150.f);

	UKismetSystemLibrary::BoxTraceSingle
	(
		this,
		BoxTraceStart,
		BoxTraceEnd,
		BoxExtent,
		CenterActor->GetActorRotation(),
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		BoxHit,
		true,
		FLinearColor::Green,
		FLinearColor::Blue,
		0.f
	);

	if (BoxHit.bBlockingHit == false)
	{
		LocationToAppear = BoxTraceStart;
		return true;
	}

	return false;
}

void AEnemyBoss::AppearFromLRB()
{
	SetActorLocation(LocationToAppear);
	SetActorRotation(RotationToAppear);

	UParticleSystemComponent* SwordParticle = Cast<UParticleSystemComponent>(GetMesh()->GetChildComponent(0));
	if (SwordParticle)
	{
		SwordParticle->Activate();
	}
	if (AppearanceParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AppearanceParticle, GetActorLocation(), GetActorRotation(), true);
	}
	if (AppearSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AppearSound, GetActorLocation());
	}

	GetMesh()->SetVisibility(true);
}

void AEnemyBoss::PlayBackAttackMontage()
{
	if (BackAttackMontage == nullptr)
		return;
	EnemyAnim->Montage_Play(BackAttackMontage);
}

void AEnemyBoss::BackAttackMontageEnd(bool IsSuccess)
{
	bIsAttacking = false;
	bAbleEvadeSkill = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, EvadeSkillCoolTime);
	}
	else
	{
		EvadeSkillCoolTimeEnd();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::Evade()
{
	EWeaponType TargetWeaponType = Target->GetCombatComp()->GetEquippedWeapon()->GetWeaponType();

	FName Dir;
	if (TargetWeaponType == EWeaponType::EWT_Gun)
	{
		int8 RanNum = FMath::RandRange(0, 1);
		Dir = RanNum ? FName("L") : FName("R");
		if (CheckObstacle(this, Dir, 180.f) == false)
		{
			Dir = RanNum ? FName("R") : FName("L");
			if (CheckObstacle(this, Dir, 180.f) == false)
			{
				EvadeMontageEnd(false);
				return;
			}
		}
	}
	else
	{
		Dir = FName("B");
		if (CheckObstacle(this, Dir, 180.f) == false)
		{
			EvadeMontageEnd(false);
			return;
		}
	}

	bEvadeSkillCoolTimeEnd = false;
	PlayEvadeMontage(Dir);
}

bool AEnemyBoss::CalculPercentage(float Percentage)
{
	return (FMath::RandRange(0.f, 1.f) <= Percentage);
}

void AEnemyBoss::PlayEvadeMontage(FName Dir)
{
	if (EvadeMontage == nullptr)
		return;
	EnemyAnim->Montage_Play(EvadeMontage);
	EnemyAnim->Montage_JumpToSection(Dir);
}

void AEnemyBoss::EvadeMontageEnd(bool IsSuccess)
{
	bIsAttacking = false;
	bAbleEvadeSkill = false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, EvadeSkillCoolTime);
	}
	else
	{
		EvadeSkillCoolTimeEnd();
	}
	OnAttackEnd.Broadcast();
}

bool AEnemyBoss::GetPhase2() const
{
	return bPhase2;
}

bool AEnemyBoss::GetRangedAttackCoolTimeEnd() const
{
	return bRangedAttackCoolTimeEnd;
}

bool AEnemyBoss::GetAbleEvadeSkill() const
{
	return bAbleEvadeSkill;
}

bool AEnemyBoss::GetDashAttack() const
{
	return bDashAttack;
}
