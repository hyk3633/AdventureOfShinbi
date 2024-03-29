

#include "EnemyBoss/EnemyBoss.h"
#include "EnemyBoss/BossAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "Player/AOSController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CombatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/RangedWeapon.h"
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

	StrafingAnimRate = 0.75f;
	ChaseAnimRate = 1.f;
	PatrolSpeed = 350.f;
	ChaseSpeed = 650.f;

	Damage = 400.f;
	Health = 5000.f;
	MaxHealth = 5000.f;
	Defense = 50.f;
	DefaultValue = 2.f;
	RandRangeValue = 10;

	AssetName = TEXT("Glacial");
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	BossController = Cast<ABossAIController>(GetController());

	PlayerController = Cast<AAOSController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
	{
		PlayerController->SetHUDBossHealthBar(GetHealthPercentage());
	}

	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyBoss::BlizzardMontageEnd);

	DashAndWallBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::OnDashBoxOverlap);

	CheckDirectionArr.Add(FName("L"));
	CheckDirectionArr.Add(FName("R"));
	CheckDirectionArr.Add(FName("B"));
}

void AEnemyBoss::SetTarget()
{
	Target = Cast<AAOSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Target)
	{
		Target->DAttackButtonPressed.BindUObject(this, &AEnemyBoss::DetectAttack);
	}

	if (BossController)
	{
		BossController->SetTarget(Target);
	}

	if (BattleMusic)
	{
		MusicComp = UGameplayStatics::SpawnSound2D(this, BattleMusic, Volume);
	}

	GetWorldTimerManager().SetTimer(FreezingCoolTimer, this, &AEnemyBoss::FreezingCoolTimeEnd, 15.f);
	GetWorldTimerManager().SetTimer(RangedAttackCoolTimer, this, &AEnemyBoss::RangedAttackCoolTimeEnd, 10.f);
	GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, 10.f);

	SetHealthBar();
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
		AiInfo.bIsPlayerDead == false &&
		Target != nullptr
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
	AAOSCharacter* Cha = Cast<AAOSCharacter>(DamageCauser);
	if (Cha)
	{
		const int32 RandInt = FMath::RandRange(1, Cha->GetCombatComp()->GetRandRangeValue());
		const float RandValue = Cha->GetCombatComp()->GetDefaultValue() * RandInt;
		float Dmg = (DamageReceived - (Defense / 2)) + RandValue;

		const bool bIsCritical = RandInt / Cha->GetCombatComp()->GetRandRangeValue() > 0.7f;
		const bool bIsHeadShot = BoneName == FName("head") ? true : false;
		Dmg = bIsHeadShot ? Dmg * 1.5f : Dmg;
		Dmg = FMath::RoundToFloat(Dmg);

		HandleHealthChange(Dmg);
		PopupDamageAmountWidget(InstigatedBy, HitLocation, Dmg, bIsHeadShot, bIsCritical);
	}

	if (GetHealthPercentage() == 0.f)
	{
		if (MusicComp)
		{
			MusicComp->Stop();
			MusicComp->DestroyComponent();
		}
		if (BossController)
		{
			BossController->UpdateAiInfo();
		}
		if (PlayerController)
		{
			PlayerController->BossHealthBarOff();
		}
		DBossDefeat.ExecuteIfBound();
	}
	else if (GetHealthPercentage() <= 0.6f && bPhase2 == false)
	{
		bPhase2 = true;
		if (Phase2Music)
		{
			MusicComp->DestroyComponent();
			MusicComp = UGameplayStatics::SpawnSound2D(this, Phase2Music, Volume);
		}
	}
}

void AEnemyBoss::SetHealthBar()
{
	if (PlayerController)
	{
		PlayerController->SetHUDBossHealthBar(GetHealthPercentage());
		PlayerController->BossHealthBarOn();
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

	// 대쉬 도중 플레이어가 콜리전 박스와 오버랩되면 플레이어가 보는 방향과 왼쪽, 오른쪽을 판별하여 플레이어가 밀쳐질 방향을 계산
	const float Dist = FVector::Dist(DashStartLocation, Cha->GetActorLocation());

	FVector ToTarget = GetActorLocation() - Cha->GetActorLocation();
	ToTarget.Normalize();
	const float Dot = FVector::DotProduct(Cha->GetActorForwardVector(), ToTarget);

	FVector Cross = FVector::CrossProduct(ToTarget, Cha->GetActorForwardVector());
	Cross.Normalize();
	const float CrossDot = FVector::DotProduct(Cross, Cha->GetActorUpVector());
	
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
	const FVector LaunchDirection = GetActorForwardVector().RotateAngleAxis(RotateValue, FVector(0, 0, 1));

	Cha->LaunchCharacter(LaunchDirection * Dist * 5.f, false, true);
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeDash);

	if (DashHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashHitSound, Cha->GetActorLocation());
	}

	UGameplayStatics::ApplyPointDamage(Cha, Dist * 0.4f, LaunchDirection, SweepResult, GetController(), this, UDamageType::StaticClass());

	SetBoxState(EBoxState::EBS_Disabled);
}

void AEnemyBoss::DetectAttack()
{
	if (bEvadeSkillCoolTimeEnd == false || bIsAttacking)
		return;

	EvadeSkillNum = (EvadeSkillNum + 1) % 3;
	
	// 타겟 플레이어의 무기 유형과 거리에 따라 다른 스킬을 발동
	ARangedWeapon* RW = Cast<ARangedWeapon>(Target->GetCombatComp()->GetEquippedWeapon());
	if (RW && GetDistanceTo(Target) >= 500.f && EvadeSkillNum == 0)
	{
		EvadeSkillNum = bPhase2 ? 1 : FMath::RandRange(1, 2);
	}
	else if(GetDistanceTo(Target) < 500.f && EvadeSkillNum == 2)
	{
		EvadeSkillNum = FMath::RandRange(0, 1);
	}
	else
	{
		return;
	}

	bAbleEvadeSkill = true;
	if (BossController)
	{
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
}

void AEnemyBoss::ResetAIState()
{
	Super::ResetAIState();

	bPhase2					 = false;
	bFreezingCoolTimeEnd	 = false;
	bRangedAttackCoolTimeEnd = false;
	bEvadeSkillCoolTimeEnd	 = false;
	bAbleEvadeSkill			 = false;
	bFreezingAttack			 = false;
	bBlizzardDebuffOn		 = false;
	bDashAttack				 = false;
	bActivateBurst			 = false;
	bAbleBackAttack			 = false;
	bAbleIceWall			 = false;

	GetWorldTimerManager().ClearTimer(FreezingCoolTimer);
	GetWorldTimerManager().ClearTimer(BlizzardDebuffTimer);
	GetWorldTimerManager().ClearTimer(DashDelayTimer);
	GetWorldTimerManager().ClearTimer(RangedAttackCoolTimer);
	GetWorldTimerManager().ClearTimer(EvadeSkillCoolTimer);
	GetWorldTimerManager().ClearTimer(BackAttackAppearTimer);
	GetWorldTimerManager().ClearTimer(DisappearTimer);
	GetWorldTimerManager().ClearTimer(IceWallAttackDisappearTimer);
	GetWorldTimerManager().ClearTimer(IceWallAttackAppearTimer);

	if (PlayerController)
	{
		PlayerController->BossHealthBarOff();
	}

	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetRangedAttackCoolTimeEnd(bRangedAttackCoolTimeEnd);
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
	if (bDeath == false && bStrafing && bIsAttacking == false && AiInfo.bStunned == false)
		return true;
	else
	{
		EndStrafing();
		return false;
	}
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
	
	// 확률에 따라 빙결 스킬 발동
	if (bFreezingCoolTimeEnd && bBlizzardDebuffOn == false && FMath::RandRange(0.f,1.f) < 0.5f)
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

	// 타겟 플레이어와의 거리에 따라 다른 스킬 발동
	RangedAttackNum = (RangedAttackNum + 1) % (bPhase2 ? 4 : 3);
	const float DistToTarget = GetDistanceTo(Target);
	if (DistToTarget > 800.f && RangedAttackNum == 0)
	{
		RangedAttackNum = FMath::RandRange(1, bPhase2 ? 3 : 2);
	}
	else if(DistToTarget <= 400.f && RangedAttackNum != 0)
	{
		RangedAttackNum = 0;
	}

	switch (RangedAttackNum)
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

	AttackNum = (AttackNum + 1) % (bPhase2 ? 3 : 2);

	if (AttackNum == 0)
	{
		PlayAttack1Montage();
	}
	else if (AttackNum == 1)
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

void AEnemyBoss::DisableCollision()
{
	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Walking;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
}

void AEnemyBoss::EnableCollision()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
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
		AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
		if (BossController)
		{
			BossController->UpdateAiInfo();
		}
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

	bFreezingHitted = true;
	Target->ActivateFreezing(true);

	// 파티클 위치 조정
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
	bFreezingAttack = false;
	GetWorldTimerManager().SetTimer(FreezingCoolTimer, this, &AEnemyBoss::FreezingCoolTimeEnd, FreezingCoolTime);

	if (bFreezingHitted)
	{
		bFreezingHitted = false;
		PlayAttackMontage();
	}
	else
	{
		bIsAttacking = false;
		bFreezingCoolTimeEnd = false;
		AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
		if (BossController)
		{
			BossController->UpdateAiInfo();
		}
		OnAttackEnd.Broadcast();
	}
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
		// 타겟 플레이어가 1000 거리 이내에 있다면 디버프 적용
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
	UGameplayStatics::ApplyDamage(Target, DeltaTime * 200.f, GetController(), this, UDamageType::StaticClass());
}

void AEnemyBoss::BlizzardDebuffEnd()
{
	bBlizzardDebuffOn = false;

	Target->SetWalkingSpeed(EWalkingState::EWS_Armed);
}

void AEnemyBoss::BlizzardMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == BlizzardMontage)
	{
		bIsAttacking = false;
		bRangedAttackCoolTimeEnd = false;
		AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
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
	// 현재 대쉬 횟수가 최대값 이하면 다시 대쉬 호출
	if (DashCount < MaxDashCount)
	{
		Dash();
	}
	else
	{
		SetBoxState(EBoxState::EBS_Disabled);
		bIsAttacking = false;
		bRangedAttackCoolTimeEnd = false;
		AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
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
	case EBoxState::EBS_Dash: // 플레이어와 오버랩되도록 설정
		DashAndWallBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DashAndWallBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DashAndWallBox->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
		DashAndWallBox->SetBoxExtent(FVector(50.f, 80.f, 150.f));
		DashAndWallBox->SetRelativeLocation(FVector(180.f, 0.f, 60.f));
		break;
	case EBoxState::EBS_Wall: // 모든 액터를 블록하도록 설정
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
	AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
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
	if (IcicleParticle == nullptr)
		return;

	bIcicleAttack = true;

	FTransform ParticleTransform;
	ParticleTransform.SetLocation(Target->GetActorLocation() + FVector(0.f, 120.f, 0.f));
	ParticleTransform.SetRotation(FRotator(0.f, 120.f, 0.f).Quaternion());
	ParticleTransform.SetScale3D(FVector(5.f, 5.f, 5.f));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IcicleParticle, ParticleTransform);
	if (IcicleSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, IcicleSound, ParticleTransform.GetLocation());
	}

	// 스페어 트레이스 수행 후 적중 시 데미지 적용
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
			UGameplayStatics::ApplyPointDamage(Target, 400.f, SphereHit.ImpactPoint, SphereHit, GetController(), this, UDamageType::StaticClass());
			Target->LaunchCharacter(Target->GetActorUpVector() * 1000.f, true, false);
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
			700.f,
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
	bIcicleAttack = false;
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
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	LocationToAppear = GetActorLocation();
	RotationToAppear = GetActorRotation();

	// 3가지 랜덤한 방향 중 장애물이 없는 방향을 나타날 방향으로 설정
	int8 RandNum = FMath::RandRange(0, 2);
	for (int8 i = 0; i < 3; i++)
	{
		if (CheckObstacle(Target, CheckDirectionArr[RandNum], 250.f))
		{
			SetRotationToAppear(CheckDirectionArr[RandNum]);
			break;
		}
		else
		{
			RandNum = (RandNum + 1) % 3;
		}
	}

	bEvadeSkillCoolTimeEnd = false;
	PlayIceWallAttackMontage();
	GetWorldTimerManager().SetTimer(IceWallAttackAppearTimer, this, &AEnemyBoss::AppearFromLRB, IceWallAttackAppearTime);
}

void AEnemyBoss::SetRotationToAppear(FName Direction)
{
	// 나타날 방향 설정
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

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

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

void AEnemyBoss::PlayerDead()
{
	Super::PlayerDead();

	Target = nullptr;
	BossController->SetTarget(nullptr);

	if (MusicComp)
	{
		MusicComp->Stop();
		MusicComp->DestroyComponent();
	}
}

void AEnemyBoss::BackAttack()
{
	if (Target == nullptr)
		return;

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	if (CheckObstacle(Target, FName("B"), 250.f))
	{
		RotationToAppear = GetActorRotation();
		RotationToAppear.Yaw = Target->GetActorRotation().Yaw;
	}
	else if(CheckObstacle(Target, FName("F"), 250.f))
	{
		RotationToAppear = GetActorRotation();
		RotationToAppear.Yaw = Target->GetActorRotation().Yaw + 180.f;
	}
	else
	{
		LocationToAppear = GetActorLocation();
		RotationToAppear = GetActorRotation();
	}

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
		BoxTraceStart += ((-CenterActor->GetActorRightVector() * Offset) + FVector(0.f, 0.f, -30.f));
		BoxExtent = FVector(150.f, 80.f, 30.f);
	}
	else if (Direction == FName("R"))
	{
		BoxTraceStart += ((CenterActor->GetActorRightVector() * Offset) + FVector(0.f, 0.f, -30.f));
		BoxExtent = FVector(150.f, 80.f, 30.f);
	}
	else if (Direction == FName("B"))
	{
		BoxTraceStart += ((-CenterActor->GetActorForwardVector() * Offset) + FVector(0.f, 0.f, -30.f));
		BoxExtent = FVector(200.f, 70.f, 30.f);
	}
	else if (Direction == FName("F"))
	{
		BoxTraceStart += ((CenterActor->GetActorForwardVector() * Offset) + FVector(0.f, 0.f, -30.f));
		BoxExtent = FVector(200.f, 70.f, 30.f);
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
		EDrawDebugTrace::None,
		BoxHit,
		true
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
	AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

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
	// 타겟 플레이어가 원거리 무기를 장착하고 있으면 왼쪽, 오른쪽으로만 회피
	ARangedWeapon* RW = Cast<ARangedWeapon>(Target->GetCombatComp()->GetEquippedWeapon());
	FName Dir;
	if (RW)
	{
		// 왼쪽 혹은 오른쪽에 장애물이 있는지 검사
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
		int8 RandNum = FMath::RandRange(0, 2);
		Dir = RandNum ? RandNum == 1 ? FName("L") : FName("R") : FName("B"); // RandNum이 0이면 B, 1이면 L, 2이면 R
		if (CheckObstacle(this, Dir, 180.f) == false)
		{
			Dir = RandNum ? RandNum == 1 ? FName("B") : FName("L") : FName("R"); // RandNum이 0이면 R, 1이면 B, 2이면 L
			if (CheckObstacle(this, Dir, 180.f) == false)
			{
				Dir = RandNum ? RandNum == 1 ? FName("R") : FName("B") : FName("L"); // RandNum이 0이면 L, 1이면 R, 2이면 B
				if (CheckObstacle(this, Dir, 180.f) == false)
				{
					// 3 방향 모두 장애물이 있으면 회피 중단
					EvadeMontageEnd(false);
					return;
				}
			}
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
	AiInfo.bTargetInAttackRange = GetAttackRange()->IsOverlappingActor(Target) ? true : false;
	if (BossController)
	{
		BossController->UpdateAiInfo();
		BossController->SetAbleEvadeSkill(bAbleEvadeSkill);
	}
	if (IsSuccess)
	{
		GetWorldTimerManager().SetTimer(EvadeSkillCoolTimer, this, &AEnemyBoss::EvadeSkillCoolTimeEnd, 3.f);
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

bool AEnemyBoss::GetIcicleAttack() const
{
	return bIcicleAttack;
}
