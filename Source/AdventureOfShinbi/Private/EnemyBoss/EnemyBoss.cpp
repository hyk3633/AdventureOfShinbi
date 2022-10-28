

#include "EnemyBoss/EnemyBoss.h"
#include "EnemyBoss/BossAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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
	DashAndWallBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StrafingAnimRate = 0.6f;
	ChaseAnimRate = 1.f;
	StrafingSpeed = 250.f;
	ChaseSpeed = 500.f;
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	Target = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Target)
	{
		Target->DAttackButtonPressed.BindUObject(this, &AEnemyBoss::DetectAttack);
	}

	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyBoss::BlizzardMontageEnd);

	DashAndWallBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::OnDashBoxOverlap);

	BossController = Cast<ABossAIController>(GetController());
}

void AEnemyBoss::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);

	RotateToTarget(DeltaTime);

	DoStrafing();

	BlizzardDotDamage(DeltaTime);

	Weapon1LineTrace();

	BurstShockWave();
}

void AEnemyBoss::RotateToTarget(float DeltaTime)
{
	if (
		bDeath == false &&
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
		UE_LOG(LogTemp, Warning, TEXT("phase2"));
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
		if (bFreezingCoolTimeEnd)
		{
			bAbleFreezing = true;
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

	const FVector ForwardVector = GetActorForwardVector();
	FVector TargetForwardVector = Cha->GetActorForwardVector();

	// 타겟의 오른쪽 왼쪽을 구분
	const FVector CrossVec = FVector::CrossProduct(TargetForwardVector, GetActorLocation() - TargetLocation);
	const float DotResult = FVector::DotProduct(CrossVec, GetActorUpVector());

	float AngleToAdd = 45.f;
	AngleToAdd = (ForwardVector + TargetForwardVector).Size() < ForwardVector.Size() ? -(AngleToAdd + 90.f) : -AngleToAdd;
	AngleToAdd = DotResult > 0 ? AngleToAdd : AngleToAdd * -1.f;

	TargetForwardVector.Normalize();
	FVector LaunchAngle = TargetForwardVector.RotateAngleAxis(AngleToAdd, Cha->GetActorUpVector());

	Cha->LaunchCharacter(LaunchAngle * (Dist * 5.f), false, true);

	UGameplayStatics::ApplyPointDamage(Cha, Damage, LaunchAngle, SweepResult, GetController(), this, UDamageType::StaticClass());
}

void AEnemyBoss::DetectAttack()
{
	// 공격 도중에도 수행될 수 있음 그렇게 되면 해당 몽타주를 중단하고 수행하기
	// 쿨타임 검사
	EWeaponType TargetWeaponType = Target->GetCombatComp()->GetEquippedWeapon()->GetWeaponType();

	if (bEvadeCoolTimeEnd)
	{
		bAbleEvade = true;
	}
	else if (bPhase2 && bBackAttackCoolTimeEnd)
	{
		bAbleBackAttack = true;
	}
	else if (bPhase2 && bIceWallCoolTimeEnd && TargetWeaponType == EWeaponType::EWT_Gun && GetDistanceTo(Target) > 1000.f)
	{
		bAbleIceWall = true;
	}
}

void AEnemyBoss::Weapon1LineTrace()
{
	if (bActivateWeaponTrace1 == false)
		return;
	
	FHitResult WeaponHitResult;
	GetLineTraceHitResult(WeaponHitResult);

	if (WeaponHitResult.bBlockingHit)
	{
		AAOSCharacter* Cha = Cast<AAOSCharacter>(WeaponHitResult.GetActor());
		if (Cha == nullptr)
			return;

		PlayMeleeAttackEffect(WeaponHitResult.ImpactPoint, WeaponHitResult.ImpactNormal.Rotation());

		UGameplayStatics::ApplyPointDamage(Cha, Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, GetController(), this, UDamageType::StaticClass());
		if (bIsAttacking)
		{
			UGameplayStatics::ApplyPointDamage(Cha, Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, GetController(), this, UDamageType::StaticClass());
		}

		if (EnemyAnim->Montage_GetIsStopped(FreezingMontage) == false)
		{
			FreezingActivate();
		}

		bActivateWeaponTrace1 = false;
	}
	
}

void AEnemyBoss::GetLineTraceHitResult(FHitResult& HitResult)
{
	const USkeletalMeshSocket* WeaponTraceStart = GetMesh()->GetSocketByName("Weapon1TraceStart");
	if (WeaponTraceStart == nullptr) return;
	const FTransform SocketStart = WeaponTraceStart->GetSocketTransform(GetMesh());

	const USkeletalMeshSocket* WeaponTraceEnd = GetMesh()->GetSocketByName("Weapon1TraceEnd");
	if (WeaponTraceEnd == nullptr) return;
	const FTransform SocketEnd = WeaponTraceEnd->GetSocketTransform(GetMesh());

	FVector TraceStart = SocketStart.GetLocation();
	FVector TraceEnd = SocketEnd.GetLocation();

	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	UKismetSystemLibrary::BoxTraceSingle
	(
		this,
		TraceStart,
		TraceEnd,
		FVector(20.f,20.f,20.f),
		SocketStart.GetRotation().Rotator(),
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Green,
		FLinearColor::Red,
		50.f
	);
}

bool AEnemyBoss::CheckStrafingCondition()
{
	return bDeath == false &&
		bStrafing &&
		bIsAttacking == false &&
		AiInfo.bTargetInAttackRange == false &&
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

void AEnemyBoss::PlayAttackMontage()
{
	if (EnemyAnim == nullptr) return;

	int8 RanNum = 0;
	RanNum = bPhase2 ? FMath::RandRange(0, 2) : FMath::RandRange(0, 1);

	if (RanNum == 0)
	{
		PlayAttack1Montage();
	}
	else if (RanNum == 1)
	{
		PlayAttack2Montage();
	}
	else
	{
		PlayAttack3Montage();
	}
}

void AEnemyBoss::PlayAttack1Montage()
{
	if (AttackMontage == nullptr) return;

	EnemyAnim->Montage_Play(AttackMontage);
}

void AEnemyBoss::PlayAttack2Montage()
{
	if (Attack2Montage == nullptr) return;

	EnemyAnim->Montage_Play(Attack2Montage);
}

void AEnemyBoss::PlayAttack3Montage()
{
	if (Attack3Montage == nullptr) return;

	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;

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
		AiInfo.bTargetInAttackRange = false;
		GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Walking;
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

void AEnemyBoss::Blizzard()
{
	bBlizzardCoolTimeEnd = false;
	bIsAttacking = true;
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

	// 타겟 속도 느리게 그리고 화면 동상 이펙트 출력
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
		GetWorldTimerManager().SetTimer(BlizzardCoolTimer, this, &AEnemyBoss::BlizzardCoolTimeEnd, BlizzardCoolTime);
		bIsAttacking = false;
		OnAttackEnd.Broadcast();
	}
}

void AEnemyBoss::BlizzardCoolTimeEnd()
{
	bBlizzardCoolTimeEnd = true;
}

void AEnemyBoss::Dash()
{
	bIsAttacking = true;
	bDashCoolTimeEnd = false;
	DashCount++;

	SetBoxState(EBoxState::EBS_Dash);

	DashStartLocation = GetActorLocation();

	PlayDashMontage();
}

void AEnemyBoss::PlayDashMontage()
{
	if (DashMontage == nullptr) 
		return;
	EnemyAnim->Montage_Play(DashMontage);
}

void AEnemyBoss::DashLaunch()
{
	float dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

	LaunchCharacter(GetActorForwardVector() * (dist * 8), false, true);
}

void AEnemyBoss::DashMontageEnd()
{
	SetBoxState(EBoxState::EBS_Disabled);

	if (DashCount < MaxDashCount)
	{
		Dash();
	}
	else
	{
		bIsAttacking = false;
		OnAttackEnd.Broadcast();
		DashCount = 0;
		GetWorldTimerManager().SetTimer(DashCoolTimer, this, &AEnemyBoss::DashCoolTimeEnd, DashCoolTime, false);
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
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DashAndWallBox->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
		DashAndWallBox->SetBoxExtent(FVector(50.f, 80.f, 150.f));
		DashAndWallBox->SetRelativeLocation(FVector(180.f, 0.f, 60.f));
		break;
	case EBoxState::EBS_Wall:
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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

void AEnemyBoss::DashCoolTimeEnd()
{
	bDashCoolTimeEnd = true;
}

void AEnemyBoss::CreateIceWall()
{
	bIsAttacking = true;
	bIceWallCoolTimeEnd = false;

	SetBoxState(EBoxState::EBS_Wall);

	if (IceWallParticle)
	{
		FTransform ParticleTransform;
		ParticleTransform.SetLocation(GetActorLocation() + FVector(50.f, 0.f, 0.f));
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

	int8 RandNum = FMath::RandRange(0, 1);

	EnemyAnim->Montage_Play(IceWallAttackMontage);
	EnemyAnim->Montage_JumpToSection(FName(*FString::FromInt(RandNum)));
}

void AEnemyBoss::IceWallAttackMontageEnd(bool IsSuccess)
{
	bAbleIceWall = false;
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(IceWallCoolTimer, this, &AEnemyBoss::IceWallCoolTimeEnd, IceWallCoolTime);
	}
	else
	{
		IceWallCoolTimeEnd();
	}
	SetBoxState(EBoxState::EBS_Disabled);
	bIsAttacking = false;
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::IceWallCoolTimeEnd()
{
	bIceWallCoolTimeEnd = true;
}

void AEnemyBoss::BackAttack()
{
	if (Target == nullptr)
		return;

	bIsAttacking = true;

	UParticleSystemComponent* SwordParticle = Cast<UParticleSystemComponent>(GetMesh()->GetChildComponent(0));
	if (SwordParticle)
	{
		UE_LOG(LogTemp, Warning, TEXT("success"));
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
	bBackAttackCoolTimeEnd = false;

	bool bSafe = CheckObstacle(Target, FName("B"), 250.f);
	if (bSafe == false)
	{
		BackAttackMontageEnd(false);
		return;
	}

	RotationToAppear = GetActorRotation();
	RotationToAppear.Yaw = Target->GetActorRotation().Yaw;

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
	bAbleBackAttack = false;
	bIsAttacking = false;
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(BackAttackCoolTimer, this, &AEnemyBoss::BackAttackCoolTimeEnd, BackAttackCoolTime);
	}
	else
	{
		BackAttackCoolTimeEnd();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::BackAttackCoolTimeEnd()
{
	bBackAttackCoolTimeEnd = true;
}

void AEnemyBoss::Evade()
{
	bIsAttacking = true;
	bEvadeCoolTimeEnd = false;
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
	bAbleEvade = false;
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(EvadeCoolTimer, this, &AEnemyBoss::EvadeCoolTimeEnd, EvadeCoolTime);
	}
	else
	{
		EvadeCoolTimeEnd();
	}
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::EvadeCoolTimeEnd()
{
	bAbleEvade = true;
}

void AEnemyBoss::EmitSwordAura()
{
	bIsAttacking = true;
	bEmitSwordAuraCoolTimeEnd = false;

	PlayFireMontage();
}

void AEnemyBoss::ProjectileFire()
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
	GetWorldTimerManager().SetTimer(EmitSwordAuraCoolTimer, this, &AEnemyBoss::EmitSwordAuraCoolTimeEnd, EmitSwordAuraCoolTime);
	bIsAttacking = false;
	OnAttackEnd.Broadcast();
}

void AEnemyBoss::EmitSwordAuraCoolTimeEnd()
{
	bEmitSwordAuraCoolTimeEnd = true;
}

void AEnemyBoss::Freezing()
{
	bIsAttacking = true;
	bFreezingCoolTimeEnd = false;

	PlayFreezingMontage();
}

void AEnemyBoss::FreezingActivate()
{
	if (Target == nullptr)
	{
		bFreezingCoolTimeEnd = true;
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
	AiInfo.bTargetInAttackRange = false;
	bAbleFreezing = false;

	GetWorldTimerManager().SetTimer(FreezingCoolTimer, this, &AEnemyBoss::FreezingCoolTimeEnd, FreezingCoolTime);

	OnAttackEnd.Broadcast();
}

void AEnemyBoss::FreezingCoolTimeEnd()
{
	bFreezingCoolTimeEnd = true;
}

void AEnemyBoss::IcicleAttack()
{
	bIsAttacking = true;
	bIcicleAttackCoolTimeEnd = false;

	PlayIcicleAttackMontage();
}

void AEnemyBoss::RisingIcicle()
{
	DrawDebugSphere(GetWorld(), IcicleLocation, 100.f, 24.f, FColor::Blue, false, 5.f);
	if (IcicleParticle)
	{
		FTransform ParticleTransform;
		ParticleTransform.SetLocation(Target->GetActorLocation() + FVector(0.f, 120.f, 0.f));
		ParticleTransform.SetRotation(FRotator(0.f,120.f,0.f).Quaternion());
		ParticleTransform.SetScale3D(FVector(5.f, 5.f, 5.f));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IcicleParticle, ParticleTransform);
		if (IcicleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, IcicleSound, ParticleTransform.GetLocation());
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
	GetLineTraceHitResult(WeaponHitResult);
	if (WeaponHitResult.bBlockingHit)
	{
		TArray<AActor*> Ignore;
		DrawDebugSphere(GetWorld(), WeaponHitResult.ImpactPoint, 100.f, 24.f, FColor::Purple, false, 5.f);
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

		// 이펙트 및 임펄스
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
	GetWorldTimerManager().SetTimer(IcicleAttackCoolTimer, this, &AEnemyBoss::IcicleAttackCoolTimeEnd, IcicleAttackCoolTime);

	OnAttackEnd.Broadcast();
}

void AEnemyBoss::IcicleAttackCoolTimeEnd()
{
	bIcicleAttackCoolTimeEnd = true;
}

void AEnemyBoss::ActivateBurst()
{
	bActivateBurst = true;
}

void AEnemyBoss::DeactivateBurst()
{
	bActivateBurst = false;
}

APawn* AEnemyBoss::GetTarget() const
{
	return Target;
}

bool AEnemyBoss::GetPhase2() const
{
	return bPhase2;
}

bool AEnemyBoss::GetAbleIceWall() const
{
	return bAbleIceWall;
}

bool AEnemyBoss::GetAbleBackAttack() const
{
	return bAbleBackAttack;
}

bool AEnemyBoss::GetAbleEvade() const
{
	return bAbleEvade;
}

bool AEnemyBoss::GetAbleFreezing() const
{
	return bAbleFreezing;
}

bool AEnemyBoss::GetBlizzardCoolTimeEnd() const
{
	return bBlizzardCoolTimeEnd;
}

bool AEnemyBoss::GetEmitSwordAuraCoolTimeEnd() const
{
	return bEmitSwordAuraCoolTimeEnd;
}

bool AEnemyBoss::GetDashCoolTimeEnd() const
{
	return bDashCoolTimeEnd;
}

bool AEnemyBoss::GetIcicleAttackCoolTimeEnd() const
{
	return bIcicleAttackCoolTimeEnd;
}
