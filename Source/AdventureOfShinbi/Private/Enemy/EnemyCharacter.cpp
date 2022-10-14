

#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
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
	GetMesh()->SetNotifyRigidBodyCollision(true);
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
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	OriginDamage = Damage;

	AIController = Cast<AEnemyAIController>(GetController());

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnAttackMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnHitReactionMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnStunMontageEnded);

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeOverlap);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeEndOverlap);
	
	OnTakePointDamage.AddDynamic(this, &AEnemyCharacter::TakePointDamage);

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsVector(FName("WaitingPosition"), WorldPatrolPoint);
		AIController->GetBlackBoard()->SetValueAsVector(FName("MoveToPoint"), GetActorLocation());
	}

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
		const USkeletalMeshSocket* WeaponTraceStart = GetMesh()->GetSocketByName("Weapon1TraceStart");
		if (WeaponTraceStart == nullptr) return;
		const FTransform SocketTransformStart = WeaponTraceStart->GetSocketTransform(GetMesh());

		const USkeletalMeshSocket* WeaponTraceEnd = GetMesh()->GetSocketByName("Weapon1TraceEnd");
		if (WeaponTraceEnd == nullptr) return;
		const FTransform SocketTransformEnd = WeaponTraceEnd->GetSocketTransform(GetMesh());

		FHitResult WeaponHitResult;
		FVector TraceStart = SocketTransformStart.GetLocation();
		FVector TraceEnd = SocketTransformEnd.GetLocation();

		GetWorld()->LineTraceSingleByChannel(WeaponHitResult, TraceStart, TraceEnd, ECC_EnemyWeaponTrace);

		if (WeaponHitResult.bBlockingHit)
		{
			PlayMeleeAttackEffect(WeaponHitResult.ImpactPoint, WeaponHitResult.ImpactNormal.Rotation());

			if (bIsAttacking && AIController)
			{
				UGameplayStatics::ApplyPointDamage(WeaponHitResult.GetActor(), Damage, WeaponHitResult.ImpactPoint, WeaponHitResult, AIController, this, UDamageType::StaticClass());
			}

			bActivateWeaponTrace1 = false;
		}
	}
}

void AEnemyCharacter::CheckIsKnockUp()
{
	if (AIController)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			EnemyAnim->StopAllMontages(0.2f);
			AIController->GetBlackBoard()->SetValueAsBool(FName("KnockUp"), true);
		}
		else
		{
			AIController->GetBlackBoard()->SetValueAsBool(FName("KnockUp"), false);
		}
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

void AEnemyCharacter::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetInAttackRange"), true);
	}
}

void AEnemyCharacter::OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetInAttackRange"), false);
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

void AEnemyCharacter::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetHitsMe"), true);

		HandleStiffAndStun(BoneName);

		AIController->GetBlackBoard()->SetValueAsVector(FName("DetectedLocation"), DamageCauser->GetActorLocation());
	}

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
	if (bDeath || AIController->GetBlackBoard()->GetValueAsBool(FName("IsAttacking"))) return;

	if (BoneName == FName("head"))
	{
		float Chances = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if (StunChance > Chances)
		{
			AIController->GetBlackBoard()->SetValueAsBool(FName("Stunned"), true);
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
			AIController->GetBlackBoard()->SetValueAsBool(FName("Stiffed"), true);
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
		if (AIController)
		{
			AIController->GetBlackBoard()->SetValueAsBool(FName("IsEnemyDead"), true);
		}
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

	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("IsAttacking"), true);
	}
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
	if (Montage == AttackMontage && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("IsAttacking"), false);
		bIsAttacking = false;
		OnAttackEnd.Broadcast();
	}
}

void AEnemyCharacter::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactionMontage && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("Stiffed"), false);
	}
}

void AEnemyCharacter::OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == StunMontage && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("Stunned"), false);
	}
}

void AEnemyCharacter::DeathMontageEnded()
{
	if(AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("IsDead"), true);
	}
	HealthWidget->SetVisibility(false);
	GetMesh()->bPauseAnims = true;
	//Destroy();
}

void AEnemyCharacter::ForgetHit()
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetHitsMe"), false);
		AIController->CheckNothingStimulus();
	}

	if (HealthWidget)
	{
		HealthWidget->SetVisibility(false);
	}
}

void AEnemyCharacter::RotateToTarget(float DeltaTime)
{
	if (
		!bDeath &&
		AIController &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("Stiffed")) &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("Stunned")) &&
		AIController->GetBlackBoard()->GetValueAsObject(FName("Target")) &&
		!AIController->GetBlackBoard()->GetValueAsBool(FName("IsPlayerDead")))
	{
		AActor* Target = Cast<AActor>(AIController->GetBlackBoard()->GetValueAsObject(FName("Target")));

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

void AEnemyCharacter::StopAttackMontage()
{
	if (EnemyAnim && AIController && !AIController->GetBlackBoard()->GetValueAsBool(FName("TargetInAttackRange")))
	{
		EnemyAnim->Montage_Stop(0.2f, AttackMontage);
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

