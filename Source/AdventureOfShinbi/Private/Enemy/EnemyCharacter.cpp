

#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "Components/CombatComponent.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	
	OnTakePointDamage.AddDynamic(this, &AEnemyCharacter::TakePointDamage);

	AiInfo.WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
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
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateToTarget(DeltaTime);

	CheckIsKnockUp();

	Weapon1LineTrace();

	Healing(DeltaTime);
}

void AEnemyCharacter::Weapon1LineTrace()
{
	if (bActivateWeaponTrace1)
	{
		FHitResult WeaponHitResult;
		GetLineTraceHitResult(WeaponHitResult);

		if (WeaponHitResult.bBlockingHit)
		{
			PlayMeleeAttackEffect(WeaponHitResult.ImpactPoint, WeaponHitResult.ImpactNormal.Rotation());

			if (bIsAttacking)
			{
				UGameplayStatics::ApplyPointDamage(WeaponHitResult.GetActor(), Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, GetController(), this, UDamageType::StaticClass());
			}

			bActivateWeaponTrace1 = false;
		}
	}
}

void AEnemyCharacter::GetLineTraceHitResult(FHitResult& HitResult)
{
	const USkeletalMeshSocket* WeaponTraceStart = GetMesh()->GetSocketByName("Weapon1TraceStart");
	if (WeaponTraceStart == nullptr) return;
	const FTransform SocketTransformStart = WeaponTraceStart->GetSocketTransform(GetMesh());

	const USkeletalMeshSocket* WeaponTraceEnd = GetMesh()->GetSocketByName("Weapon1TraceEnd");
	if (WeaponTraceEnd == nullptr) return;
	const FTransform SocketTransformEnd = WeaponTraceEnd->GetSocketTransform(GetMesh());

	FVector TraceStart = SocketTransformStart.GetLocation();
	FVector TraceEnd = SocketTransformEnd.GetLocation();

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_EnemyWeaponTrace);
}

void AEnemyCharacter::CheckIsKnockUp()
{
	if (GetCharacterMovement()->IsFalling())
	{
		EnemyAnim->StopAllMontages(0.2f);
		AiInfo.bIsKnockUp = true;
	}
	else
	{
		AiInfo.bIsKnockUp = false;
	}
}

void AEnemyCharacter::Healing(float DeltaTime)
{
	if (!bHealing) return;

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
		BuffParticleComponent->Deactivate();
	}
}

void AEnemyCharacter::OnDetected(AActor* Actor, FAIStimulus Stimulus)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(Actor);
	if (Cha == nullptr) return;

	if (IsPlayerDeathDelegateBined == false)
	{
		Cha->GetCombatComp()->PlayerDeathDelegate.AddLambda([this]() -> void {
			AiInfo.bIsPlayerDead = true;
		});
	}

	if (Stimulus.Type.Name == FName("Default__AISense_Sight"))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AiInfo.bTargetIsVisible = true;
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
	else if (Stimulus.Type.Name == FName("Default__AISense_Hearing"))
	{
		if (!Stimulus.IsExpired())
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
	if (AiInfo.bTargetIsVisible == false &&
		AiInfo.bSightStimulusExpired &&
		AiInfo.bTargetIsHeard == false &&
		AiInfo.bTargetHitsMe == false)
	{
		if (EnemyState == EEnemyState::EES_Chase || EnemyState == EEnemyState::EES_Detected)
		{
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

	HandleStiffAndStun(BoneName);
	HandleHealthChange(DamageReceived);
	PopupDamageAmountWidget(InstigatedBy, HitLocation, DamageReceived, BoneName);
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

void AEnemyCharacter::HandleStiffAndStun(FName& BoneName)
{
	if (bDeath || bIsAttacking) return;

	if (BoneName == FName("head"))
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StunChance > Chances)
		{
			AiInfo.bStunned = true;
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
}

void AEnemyCharacter::HandleHealthChange(float DamageReceived)
{
	if (bDeath) return;

	Health = FMath::Clamp(Health - DamageReceived, 0.f, MaxHealth);

	GetWorldTimerManager().SetTimer(HitForgetTimer, this, &AEnemyCharacter::ForgetHit, HitMemoryTime);

	if (HealthWidget)
	{
		SetHealthBar();
		HealthWidget->SetVisibility(true);
	}

	if (Health == 0)
	{
		bDeath = true;

		// Á×À½ ÀÌÆåÆ® Àç»ý
		if (DeathParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticle, GetActorLocation());
		}
		if (DeathVoice)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathVoice, GetActorLocation());
		}
		PlayDeathMontage();
	}
}

void AEnemyCharacter::PopupDamageAmountWidget(AController* InstigatorController, FVector PopupLocation, float DamageNumber, FName HittedBoneName)
{
	if (bDeath) return;

	APlayerController* PlayerController = Cast<APlayerController>(InstigatorController);

	if (PlayerController && DamageAmountTextClass)
	{
		FVector2D ScreenLocation;
		PlayerController->ProjectWorldLocationToScreen(PopupLocation, ScreenLocation);

		UDamageAmount* DamageAmount = CreateWidget<UDamageAmount>(PlayerController, DamageAmountTextClass);

		if (DamageAmount)
		{
			DamageAmount->DamageText->SetText(FText::AsNumber(DamageNumber));
			if (HittedBoneName == FName("head"))
			{
				FSlateColor Color = FSlateColor(FLinearColor(1.f, 0.f, 0.f));
				DamageAmount->DamageText->SetColorAndOpacity(Color);
			}

			DamageAmount->AddToViewport();
			DamageAmount->SetPositionInViewport(ScreenLocation);
			DamageAmount->PlayAnimation(DamageAmount->Popup);
		}
	}
}

void AEnemyCharacter::Attack()
{
	if (bDeath) return;

	bIsAttacking = true;
	PlayAttackMontage();
}

void AEnemyCharacter::PlayAttackMontage()
{
	if (EnemyAnim == nullptr || AttackMontage == nullptr) return;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(AttackMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(AttackMontage);

	EnemyAnim->Montage_JumpToSection(AttackMontageSectionNameArr[RandSectionNum]);
}

void AEnemyCharacter::PlayHitReactionMontage()
{
	if (EnemyAnim == nullptr || HitReactionMontage == nullptr) return;

	EnemyAnim->Montage_Play(HitReactionMontage);

	EnemyAnim->Montage_JumpToSection(FName("HitFront"));
}

void AEnemyCharacter::PlayStunMontage()
{
	if (EnemyAnim == nullptr || StunMontage == nullptr) return;

	EnemyAnim->Montage_Play(StunMontage);
}

void AEnemyCharacter::PlayDeathMontage()
{
	if (EnemyAnim == nullptr || DeathMontage == nullptr) return;

	EnemyAnim->Montage_Play(DeathMontage);
}

void AEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
		AiInfo.bTargetInAttackRange = false;
		OnAttackEnd.Broadcast();
	}
}

void AEnemyCharacter::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactionMontage)
	{
		AiInfo.bStiffed = false;
	}
}

void AEnemyCharacter::OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == StunMontage)
	{
		AiInfo.bStunned = false;
	}
}

void AEnemyCharacter::DeathMontageEnded()
{
	HealthWidget->SetVisibility(false);
	GetMesh()->bPauseAnims = true;
	//Destroy();
}

void AEnemyCharacter::ForgetHit()
{
	AiInfo.bTargetHitsMe = false;

	CheckNothingStimulus();
	
	if (HealthWidget)
	{
		HealthWidget->SetVisibility(false);
	}
}

void AEnemyCharacter::RotateToTarget(float DeltaTime)
{
	if (
		bDeath == false &&
		AiInfo.bStiffed == false &&
		AiInfo.bStunned == false &&
		AiInfo.TargetPlayer != nullptr &&
		AiInfo.bIsPlayerDead == false
		)
	{
		AActor* Target = Cast<AActor>(AiInfo.TargetPlayer);

		FRotator Rotation = GetActorRotation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
		LookAtRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

		float RotateRate = bIsAttacking ? 10.f : 20.f;

		Rotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotateRate);
		SetActorRotation(Rotation);
	}
}

void AEnemyCharacter::SetHealthBar()
{
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
	Damage = OriginDamage;
	BuffParticleComponent->Deactivate();
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

void AEnemyCharacter::SightStimulusExpire()
{
	AiInfo.bSightStimulusExpired = true;
	AiInfo.TargetPlayer = nullptr;

	CheckNothingStimulus();
}

void AEnemyCharacter::SetEnemyState(EEnemyState State)
{
	if (EnemyState != EEnemyState::EES_Siege) return;

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

FVector AEnemyCharacter::GetPatrolPoint() const
{
	return PatrolPoint;
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

bool AEnemyCharacter::GetIsDead() const
{
	return bDeath;
}

float AEnemyCharacter::GetEnemyDamage() const
{
	return Damage;
}

float AEnemyCharacter::GetAcceptableRaius() const
{
	return AcceptableRadius;
}

EEnemyState AEnemyCharacter::GetEnemyState() const
{
	return EnemyState;
}

