

#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/EnemyAIController.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/AOSGameModeBase.h"
#include "Components/SphereComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HUD/EnemyHealthBar.h"
#include "HUD/DamageAmount.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetNotifyRigidBodyCollision(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerWeaponTrace, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerWeaponTrace, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyWeaponTrace, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ItemRange, ECollisionResponse::ECR_Ignore);

	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(RootComponent);
	AttackRange->SetGenerateOverlapEvents(true);
	AttackRange->SetCollisionObjectType(ECC_EnemyAttackRange);
	AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRange->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	AttackRange->SetSphereRadius(200.f);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	HealthWidget->SetupAttachment(RootComponent);
	HealthWidget->SetVisibility(false);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dissolve Timeline Component"));

	/** Perception Component */

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(5.f);

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->SetMaxAge(10.f);

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->ConfigureSense(*HearingConfig);

	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	BoxTraceSize = FVector(20.f, 20.f, 20.f);

	WeaponTraceStartSocketName = FName("Weapon1TraceStart");
	WeaponTraceEndSocketName = FName("Weapon1TraceEnd");
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	OriginDamage = Damage;

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::OnDetected);

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnAttackMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnHitReactionMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnStunMontageEnded);

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeOverlap);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeOverlapEnd);

	OnTakePointDamage.AddDynamic(this, &AEnemyCharacter::TakePointDamage);

	PatrolPoint = GetActorLocation();

	AiInfo.DetectedLocation = FVector::ZeroVector;
	AiInfo.TargetPlayer = nullptr;
	AiInfo.bTargetIsVisible = false;
	AiInfo.bTargetIsHeard = false;
	AiInfo.bSightStimulusExpired = true;
	AiInfo.bIsKnockUp = false;
	AiInfo.bStunned = false;
	AiInfo.bStiffed = false;
	AiInfo.bTargetInAttackRange = false;
	AiInfo.bTargetHitsMe = false;

	AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->UpdateAiInfo();
		AIController->SetTarget(AiInfo.TargetPlayer);
		AIController->SetDetectedLocation(AiInfo.DetectedLocation);
	}

	GetWorld()->GetAuthGameMode<AAOSGameModeBase>()->DPlayerRespawn.AddUFunction(this, FName("ResetAIState"));

	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateToTarget(DeltaTime);

	CheckIsKnockUp();

	Weapon1BoxTrace();

	Healing(DeltaTime);
}

bool AEnemyCharacter::Weapon1BoxTrace()
{
	if (bActivateWeaponTrace1)
	{
		FHitResult WeaponHitResult;
		GetBoxTraceHitResult(WeaponHitResult, WeaponTraceStartSocketName, WeaponTraceEndSocketName);

		if (WeaponHitResult.bBlockingHit)
		{
			if (WeaponHitResult.GetActor() == AiInfo.TargetPlayer)
			{
				PlayMeleeAttackEffect(WeaponHitResult.ImpactPoint, WeaponHitResult.ImpactNormal.Rotation());

				UGameplayStatics::ApplyPointDamage(WeaponHitResult.GetActor(), Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, GetController(), this, UDamageType::StaticClass());

				bActivateWeaponTrace1 = false;

				return true;
			}
		}
	}

	return false;
}

void AEnemyCharacter::GetBoxTraceHitResult(FHitResult& HitResult, FName StartSocketName, FName EndSocketName)
{
	const USkeletalMeshSocket* WeaponTraceStart = GetMesh()->GetSocketByName(StartSocketName);
	if (WeaponTraceStart == nullptr)
		return;
	const FTransform SocketTransformStart = WeaponTraceStart->GetSocketTransform(GetMesh());

	const USkeletalMeshSocket* WeaponTraceEnd = GetMesh()->GetSocketByName(EndSocketName);
	if (WeaponTraceEnd == nullptr) 
		return;
	const FTransform SocketTransformEnd = WeaponTraceEnd->GetSocketTransform(GetMesh());

	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_EnemyWeaponTrace);

	UKismetSystemLibrary::BoxTraceSingle
	(
		this,
		SocketTransformStart.GetLocation(),
		SocketTransformEnd.GetLocation(),
		BoxTraceSize,
		FRotator::ZeroRotator,
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	);
}

void AEnemyCharacter::CheckIsKnockUp()
{
	// 공중에 띄워졌으면 모든 애니메이션 및 공격 중단
	if (GetCharacterMovement()->IsFalling() && bDeath == false)
	{
		EnemyAnim->StopAllMontages(0.2f);
		AiInfo.bIsKnockUp = true;
		AbortAttack();
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
	}
	else if(AiInfo.bIsKnockUp)
	{
		if (GetWorldTimerManager().IsTimerActive(KnockUpDelayTimer) == false)
		{
			GetWorldTimerManager().SetTimer(KnockUpDelayTimer, this, &AEnemyCharacter::EndKnockUpDelay, 2.f);
		}
	}
}

void AEnemyCharacter::EndKnockUpDelay()
{
	AiInfo.bIsKnockUp = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}

void AEnemyCharacter::AbortAttack()
{
	if (bIsAttacking)
	{
		bIsAttacking = false;

		// 타겟이 근접 공격 콜리전에 계속 오버랩되있으면 true
		if (GetAttackRange()->IsOverlappingActor(AiInfo.TargetPlayer) == false)
			AiInfo.bTargetInAttackRange = false;
		else
			AiInfo.bTargetInAttackRange = true;
		OnAttackEnd.Broadcast();
	}
}

void AEnemyCharacter::Healing(float DeltaTime)
{
	if (!bHealing) 
		return;

	const float Amount = DeltaTime * RecoveryRate;
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);

	if (HealthWidget)
	{
		SetHealthBar();
	}

	HealedAmount += Amount;
	if (FMath::FloorToFloat(HealedAmount) >= HealAmount)
	{
		Health = FMath::FloorToFloat(Health);
		HealAmount = 0.f;
		HealedAmount = 0.f;
		bHealing = false;
		HealthWidget->SetVisibility(false);
		BuffEnd();
	}
}

void AEnemyCharacter::OnDetected(AActor* Actor, FAIStimulus Stimulus)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(Actor);
	if (Cha == nullptr) 
		return;

	if (IsPlayerDeathDelegateBined == false)
	{
		// 감지된 타겟 플레이어의 사망 델리게이트에 함수 바인딩
		Cha->GetCombatComp()->PlayerDeathDelegate.AddDynamic(this, &AEnemyCharacter::PlayerDead);
		IsPlayerDeathDelegateBined = true;
	}

	// 시각 자극 갱신
	if (Stimulus.Type.Name == FName("Default__AISense_Sight"))
	{
		// 시각 자극이 감지되었을 경우
		if (Stimulus.WasSuccessfullySensed())
		{
			AiInfo.bTargetIsVisible = true;
			AiInfo.bSightStimulusExpired = false;
			AiInfo.TargetPlayer = Cha;

			SetEnemyState(EEnemyState::EES_Chase);

			GetWorldTimerManager().ClearTimer(SightStimulusExpireTimer);
		}
		else
		{
			AiInfo.bTargetIsVisible = false;
			GetWorldTimerManager().SetTimer(SightStimulusExpireTimer, this, &AEnemyCharacter::SightStimulusExpire, SightStimulusExpireTime);
		}
	}
	else if (Stimulus.Type.Name == FName("Default__AISense_Hearing")) // 청각 자극 갱신
	{
		// 청각 자극이 감지되었을 경우
		if (Stimulus.IsExpired() == false)
		{
			AiInfo.bTargetIsHeard = true;
			AiInfo.DetectedLocation = Cha->GetActorLocation();

			if (EnemyState == EEnemyState::EES_Patrol)
			{
				SetEnemyState(EEnemyState::EES_Detected);
			}
		}
		else
		{
			AiInfo.bTargetIsHeard = false;
		}
	}

	// AI 상태 정보 갱신
	if (AIController)
	{
		AIController->UpdateAiInfo();
		AIController->SetTarget(AiInfo.TargetPlayer);
		AIController->SetDetectedLocation(AiInfo.DetectedLocation);
	}

	CheckNothingStimulus();
}

void AEnemyCharacter::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsAttacking)
		return;

	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		AiInfo.bTargetInAttackRange = true;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
	}
}

void AEnemyCharacter::OnAttackRangeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 플레이어가 공격 범위 밖으로 나가도 공격 도중이면 해당 bool 값을 false로 설정하지 않음 (공격 태스크가 중단되므로)
	if (bIsAttacking)
		return;

	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha)
	{
		AiInfo.bTargetInAttackRange = false;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
	}
}

void AEnemyCharacter::PlayMeleeAttackEffect(FVector HitLocation, FRotator HitRotation)
{
	if (MeleeHitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, MeleeHitParticle, HitLocation, HitRotation);
	}
	if (MeleeHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MeleeHitSound, HitLocation);
	}
}

void AEnemyCharacter::ActivateHealing(float RecoveryAmount)
{
	PlayBuffParticle();
	HealAmount = RecoveryAmount;
	bHealing = true;
	HealthWidget->SetVisibility(true);
}

void AEnemyCharacter::ActivateDamageUp(float DamageUpRate)
{
	PlayBuffParticle();
	Damage *= DamageUpRate;
	GetWorldTimerManager().SetTimer(DamageUpTimer, this, &AEnemyCharacter::DamageUpTimeEnd, DamageUpTime);
}

void AEnemyCharacter::CheckNothingStimulus()
{
	if (AiInfo.bTargetIsVisible == false && // 타겟 플레이어가 보이지 않고
		AiInfo.bSightStimulusExpired &&		// 타겟 플레이어에 대한 시각 기억이 만료되었고
		AiInfo.bTargetIsHeard == false &&	// 타겟 플레이어의 노이즈가 감지 되지 않고
		AiInfo.bTargetHitsMe == false)		// 타겟 플레이어에게 피격 당하지 않았거나 피격 자극이 만료되었을 경우
	{
		if (EnemyState == EEnemyState::EES_Chase || EnemyState == EEnemyState::EES_Detected)
		{
			// 움직임 상태를 복귀로 설정
			SetEnemyState(EEnemyState::EES_Comeback);
		}
	}
}

void AEnemyCharacter::SetStateToPatrol()
{
	if (EnemyState == EEnemyState::EES_Comeback)
	{
		SetEnemyState(EEnemyState::EES_Patrol);
	}
}

void AEnemyCharacter::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	AiInfo.bTargetHitsMe = true;
	AiInfo.DetectedLocation = DamageCauser->GetActorLocation();
	if (AIController)
	{
		AIController->UpdateAiInfo();
		AIController->SetDetectedLocation(AiInfo.DetectedLocation);
	}

	if (AiInfo.bTargetIsVisible == false)
	{
		if (EnemyState == EEnemyState::EES_Patrol)
		{
			SetEnemyState(EEnemyState::EES_Detected);
		}
	}

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

		HandleStiffAndStun(bIsHeadShot);
		HandleHealthChange(Dmg);
		PopupDamageAmountWidget(InstigatedBy, HitLocation, Dmg, bIsHeadShot, bIsCritical);
	}
}

void AEnemyCharacter::PlayHitEffect(FVector HitLocation, FRotator HitRotation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, HitRotation);
	}
	if (HitVoice)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitVoice, HitLocation);
	}
}

void AEnemyCharacter::HandleStiffAndStun(bool IsHeadShot)
{
	if (bDeath) 
		return;

	if (IsHeadShot)
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StunChance > Chances)
		{
			AiInfo.bStunned = true;
			AbortAttack();
			PlayStunMontage();
		}
		else
		{
			if (EnemyState == EEnemyState::EES_Patrol)
			{
				SetEnemyState(EEnemyState::EES_Detected);
			}
		}
	}
	else
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StiffChance > Chances)
		{
			AiInfo.bStiffed = true;
			AbortAttack();
			PlayHitReactionMontage();
		}
		else
		{
			if (EnemyState == EEnemyState::EES_Patrol)
			{
				SetEnemyState(EEnemyState::EES_Detected);
			}
		}
	}

	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}

void AEnemyCharacter::HandleHealthChange(float DamageReceived)
{
	if (bDeath) 
		return;

	Health = FMath::Clamp(Health - DamageReceived, 0.f, MaxHealth);

	GetWorldTimerManager().SetTimer(HitForgetTimer, this, &AEnemyCharacter::ForgetHit, HitMemoryTime);

	SetHealthBar();

	if (Health == 0)
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMovementComponent()->StopMovementImmediately();
		bDeath = true;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
		PlayDeathMontage();
		BuffEnd();
	}
}

void AEnemyCharacter::PopupDamageAmountWidget(AController* InstigatorController, FVector PopupLocation, float DamageNumber, bool IsHeadShot, bool IsCritical)
{
	if (bDeath) 
		return;

	APlayerController* PlayerController = Cast<APlayerController>(InstigatorController);

	if (PlayerController && DamageAmountTextClass)
	{
		FVector2D ScreenLocation;
		PlayerController->ProjectWorldLocationToScreen(PopupLocation, ScreenLocation);
		ScreenLocation.X += FMath::RandRange(-5.f, 5.f);
		ScreenLocation.Y += FMath::RandRange(-5.f, 5.f);

		UDamageAmount* DamageAmount = CreateWidget<UDamageAmount>(PlayerController, DamageAmountTextClass);

		if (DamageAmount)
		{
			// 헤드샷, 크리티컬 여부에 따라 크기와 색을 다르게 출력
			DamageAmount->DamageText->SetText(FText::AsNumber(DamageNumber));
			if (IsHeadShot)
			{
				DamageAmount->DamageText->SetColorAndOpacity(FLinearColor::Red);
			}
			else if (IsCritical)
			{
				DamageAmount->DamageText->SetColorAndOpacity(FLinearColor(1.f,0.3f,0.f));
			}
			else
			{
				DamageAmount->DamageText->SetColorAndOpacity(FLinearColor(1.f, 1.f, 0.f));
			}

			if (IsCritical)
			{
				DamageAmount->DamageText->SetRenderScale(FVector2D(1.5f, 1.5f));
			}

			DamageAmount->AddToViewport();
			DamageAmount->SetPositionInViewport(ScreenLocation);
			DamageAmount->PlayAnimation(DamageAmount->Popup);
		}
	}
}

void AEnemyCharacter::Attack()
{
	if (bDeath) 
		return;

	bIsAttacking = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	PlayAttackMontage();
}

void AEnemyCharacter::ResetAIState()
{
	IsPlayerDeathDelegateBined = false;
	AiInfo.TargetPlayer = nullptr;
	AiInfo.DetectedLocation = FVector::ZeroVector;
	AiInfo.bTargetIsVisible = false;
	AiInfo.bTargetIsHeard = false;
	AiInfo.bSightStimulusExpired = true;
	AiInfo.bIsKnockUp = false;
	AiInfo.bStunned = false;
	AiInfo.bStiffed = false;
	AiInfo.bTargetInAttackRange = false;
	AiInfo.bTargetHitsMe = false;
	AiInfo.bIsPlayerDead = false;
	bIsAttacking = false;
	Health = MaxHealth;

	Health = MaxHealth;
	SetHealthBar();
	HealthWidget->SetVisibility(false);

	if (AIController)
	{
		AIController->UpdateAiInfo();
	}

	SetActorLocation(InitialLocation);
	SetActorRotation(InitialRotation);
}

void AEnemyCharacter::PlayAttackMontage()
{
	if (EnemyAnim == nullptr || AttackMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(AttackMontage);

	if (AttackMontageSectionNameArr.Num() > 0)
	{
		int8 RandSectionNum = UKismetMathLibrary::RandomInteger(AttackMontageSectionNameArr.Num());
		EnemyAnim->Montage_JumpToSection(AttackMontageSectionNameArr[RandSectionNum]);
	}
}

void AEnemyCharacter::PlayHitReactionMontage()
{
	if (EnemyAnim == nullptr || HitReactionMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(HitReactionMontage);
	EnemyAnim->Montage_JumpToSection(DistinguishHitDirection());
}

FName AEnemyCharacter::DistinguishHitDirection()
{
	const FVector TargetLocation = AiInfo.DetectedLocation;
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

void AEnemyCharacter::PlayStunMontage()
{
	if (EnemyAnim == nullptr || StunMontage == nullptr) 
		return;

	EnemyAnim->Montage_Play(StunMontage);
}

void AEnemyCharacter::PlayDeathMontage()
{
	if (EnemyAnim == nullptr || DeathMontage == nullptr) 
		return;

	EnemyAnim->StopAllMontages(0.f);
	EnemyAnim->Montage_Play(DeathMontage);

	if (DissolveMatInst.Num() == 0)
		return;

	for (int8 i = 0; i < DissolveMatInst.Num(); i++)
	{
		DynamicDissolveMatInst.Add(nullptr);
		if (DissolveMatInst[i])
		{
			// 캐릭터 메쉬의 머티리얼을 소멸 효과가 적용된 머티리얼로 설정
			DynamicDissolveMatInst[i] = UMaterialInstanceDynamic::Create(DissolveMatInst[i], this);
			GetMesh()->SetMaterial(i, DynamicDissolveMatInst[i]);
			DynamicDissolveMatInst[i]->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
			DynamicDissolveMatInst[i]->SetScalarParameterValue(TEXT("Glow"), 100.f);
		}
	}
	Dissolution();
}

void AEnemyCharacter::Dissolution()
{
	// 타임라인 플로트에 값을 매개변수로 받는 함수 바인딩
	DissolveTrack.BindDynamic(this, &AEnemyCharacter::UpdateDissolveMat);
	if (DissolveCurve && DissolveTimeline)
	{
		// 타임라인에 커브를 할당하고 시작
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AEnemyCharacter::UpdateDissolveMat(float Value)
{
	for (int8 i = 0; i < DynamicDissolveMatInst.Num(); i++)
	{
		if (DynamicDissolveMatInst[i])
		{
			// 커브 값에 따라 머티리얼의 파라미터 값 갱신
			DynamicDissolveMatInst[i]->SetScalarParameterValue(TEXT("Dissolve"), Value);
		}
	}
}

void AEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		GetWorldTimerManager().SetTimer(AttackDelayTimer, this, &AEnemyCharacter::AttackDelay, 2.f);
	}
}

void AEnemyCharacter::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactionMontage)
	{
		GetWorldTimerManager().SetTimer(StiffDelayTimer, this, &AEnemyCharacter::EndStiffDelay, 0.3f);
	}
}

void AEnemyCharacter::EndStiffDelay()
{
	AiInfo.bStiffed = false;
	AiInfo.bTargetInAttackRange = AttackRange->IsOverlappingActor(AiInfo.TargetPlayer) ? true : false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}

void AEnemyCharacter::OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == StunMontage)
	{
		AiInfo.bStunned = false;
		AiInfo.bTargetInAttackRange = AttackRange->IsOverlappingActor(AiInfo.TargetPlayer) ? true : false;
		if (AIController)
		{
			AIController->UpdateAiInfo();
		}
	}
}

void AEnemyCharacter::DeathMontageEnded()
{
	HealthWidget->SetVisibility(false);
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::DestroyCharacter, DestroyTime);
}

void AEnemyCharacter::DestroyCharacter()
{
	Destroy();
}

void AEnemyCharacter::ForgetHit()
{
	AiInfo.bTargetHitsMe = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
	CheckNothingStimulus();
	
	if (HealthWidget)
	{
		HealthWidget->SetVisibility(false);
	}
}

void AEnemyCharacter::RotateToTarget(float DeltaTime)
{
	if (CheckRotateToTargetCondition()
)
	{
		AActor* Target = Cast<AActor>(AiInfo.TargetPlayer);

		FRotator Rotation = GetActorRotation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		LookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

		// 공격 중 이면 회전 속도 느리게
		float RotateRate = bIsAttacking ? AttackingRotateRate : NormalRotateRate;

		Rotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotateRate);
		SetActorRotation(Rotation);
	}
}

bool AEnemyCharacter::CheckRotateToTargetCondition()
{
	return bDeath == false &&					// 캐릭터가 살아있고
		AiInfo.bStiffed == false &&				// 경직 중 이지 않고
		AiInfo.bStunned == false &&				// 스턴 중 이지 않고
		AiInfo.TargetPlayer != nullptr &&		// 타겟 플레이어가 설정 되어 있고
		AiInfo.bIsPlayerDead == false &&		// 타겟 플레이어가 살아있고
		EnemyState == EEnemyState::EES_Chase;	// 상태가 추격 중 이면 true (타겟 플레이어를 향해 회전) 
}

void AEnemyCharacter::SetHealthBar()
{
	if (HealthWidget)
	{
		HealthWidget->SetVisibility(true);
	}

	UEnemyHealthBar* Bar = Cast<UEnemyHealthBar>(HealthWidget->GetWidget());
	if (Bar)
	{
		Bar->EnemyHealthProgressBar->SetPercent(Health / MaxHealth);
	}
}

void AEnemyCharacter::ActivateWeaponTrace1()
{
	bActivateWeaponTrace1 = true;
}

void AEnemyCharacter::DeactivateWeaponTrace1()
{
	bActivateWeaponTrace1 = false;
}

void AEnemyCharacter::StopAttackMontage()
{
	if (bActivateWeaponTrace1)
	{
		bActivateWeaponTrace1 = false;
		if (EnemyAnim)
		{
			EnemyAnim->Montage_Stop(0.2f, AttackMontage);
		}
	}
}

void AEnemyCharacter::DamageUpTimeEnd()
{
	BuffEnd();
	Damage = OriginDamage;
}

void AEnemyCharacter::PlayBuffParticle()
{
	if (BuffStartParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BuffStartParticle, GetActorLocation(), GetActorRotation(), true);
	}
	if (BuffParticle)
	{
		BuffParticleComponent = UGameplayStatics::SpawnEmitterAttached
		(
			BuffParticle,
			GetMesh(),
			FName(),
			FVector::UpVector * 50.f,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset
		);
	}
}

void AEnemyCharacter::BuffEnd()
{
	if (BuffParticleComponent)
	{
		BuffParticleComponent->Deactivate();
	}
}

void AEnemyCharacter::SightStimulusExpire()
{
	AiInfo.bSightStimulusExpired = true;
	AiInfo.TargetPlayer = nullptr;
	if (AIController)
	{
		AIController->UpdateAiInfo();
		AIController->SetTarget(AiInfo.TargetPlayer);
	}
	CheckNothingStimulus();
}

void AEnemyCharacter::SetEnemyState(EEnemyState State)
{
	EnemyState = State;

	if (EnemyState == EEnemyState::EES_Patrol || EnemyState == EEnemyState::EES_Detected)
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	}
}

bool AEnemyCharacter::GetIsAttacking() const
{
	return bIsAttacking;
}

FVector AEnemyCharacter::GetPatrolPoint()
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
}

float AEnemyCharacter::GetHealthPercentage() const
{
	return Health / MaxHealth;
}

float AEnemyCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

FAiInfo AEnemyCharacter::GetAiInfo() const
{
	return AiInfo;
}

void AEnemyCharacter::SetDamage(float DamageUpRate)
{
	if (DamageUpRate > 0.f)
	{
		Damage *= DamageUpRate;
	}
	else
	{
		Damage = OriginDamage;
	}
}

float AEnemyCharacter::GetHealth() const
{
	return Health;
}

float AEnemyCharacter::GetDefaultValue() const
{
	return DefaultValue;
}

int32 AEnemyCharacter::GetRandRangeValue() const
{
	return RandRangeValue;
}

bool AEnemyCharacter::GetIsDead() const
{
	return bDeath;
}

float AEnemyCharacter::GetEnemyDamage() const
{
	return Damage;
}

USphereComponent* AEnemyCharacter::GetAttackRange() const
{
	return AttackRange;
}

void AEnemyCharacter::PlayerDead()
{
	AiInfo.bIsPlayerDead = true;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}
}

void AEnemyCharacter::AttackDelay()
{
	bIsAttacking = false;
	if (AttackRange->IsOverlappingActor(AiInfo.TargetPlayer) == false)
		AiInfo.bTargetInAttackRange = false;
	if (AIController)
	{
		AIController->UpdateAiInfo();
	}

	OnAttackEnd.Broadcast();
}

EEnemyState AEnemyCharacter::GetEnemyState() const
{
	return EnemyState;
}

