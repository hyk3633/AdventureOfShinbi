

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
#include "Components/BoxComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
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
	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerWeapon, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyWeapon, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_FindItem, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);

	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(RootComponent);
	AttackRange->SetGenerateOverlapEvents(true);
	AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRange->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	AttackRange->SetSphereRadius(200.f);

	DamageCollision1 = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision 1"));
	DamageCollision1->SetupAttachment(GetMesh(), FName("EnemySocket1"));
	DamageCollision1->SetCollisionObjectType(ECC_EnemyWeapon);
	DamageCollision1->SetGenerateOverlapEvents(false);
	DamageCollision1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageCollision1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageCollision1->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);

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

	AIController = Cast<AEnemyAIController>(GetController());

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnAttackMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnHitReactionMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnStunMontageEnded);

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeOverlap);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeEndOverlap);
	
	DamageCollision1->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnDamageCollisionOverlap);

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

void AEnemyCharacter::OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* PlayerCharacter = Cast<AAOSCharacter>(OtherActor);
	if (PlayerCharacter && bIsAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("overlap"));
		UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, GetController(), this, UDamageType::StaticClass());
	}
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
	if (HitParticle && HitSound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation, HitRotation);
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
	}
}

void AEnemyCharacter::HandleStiffAndStun(FName& BoneName)
{
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
	Health = FMath::Clamp(Health - DamageReceived, 0.f, MaxHealth);

	GetWorldTimerManager().SetTimer(HitForgetTimer, this, &AEnemyCharacter::ForgetHit, HitMemoryTime);

	if (HealthWidget)
	{
		SetHealthBar();
		HealthWidget->SetVisibility(true);
	}

	if (Health == 0)
	{
		// Á×À½ ÀÌÆåÆ® Àç»ý
		if (DeathParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticle, GetActorLocation());
		}
		if (DeathSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
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
	GetMesh()->bPauseAnims = true;
	Destroy();
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

void AEnemyCharacter::ActivateDamageCollision1()
{
	DamageCollision1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyCharacter::DeactivateDamageCollision1()
{
	DamageCollision1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

float AEnemyCharacter::GetAcceptableRaius() const
{
	return AcceptableRadius;
}

EEnemyState AEnemyCharacter::GetEnemyState() const
{
	return EnemyState;
}

