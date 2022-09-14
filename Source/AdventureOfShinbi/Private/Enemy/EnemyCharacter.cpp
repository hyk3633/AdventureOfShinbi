

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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/EnemyHealthBar.h"
#include "HUD/DamageAmount.h"
#include "DrawDebugHelpers.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetMesh()->SetCollisionObjectType(ECC_Enemy);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);

	ChaseRange = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseRange"));
	ChaseRange->SetupAttachment(RootComponent);
	ChaseRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChaseRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChaseRange->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	ChaseRange->SetSphereRadius(1000.f);

	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(RootComponent);
	AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackRange->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	AttackRange->SetSphereRadius(200.f);

	DamageCollision1 = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollision 1"));
	DamageCollision1->SetupAttachment(GetMesh(), FName("EnemySocket1"));
	DamageCollision1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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

	IsAttacking = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AEnemyAIController>(GetController());

	EnemyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnAttackMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnHitReactMontageEnded);
	EnemyAnim->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnStunMontageEnded);

	ChaseRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnChaseRangeOverlap);
	ChaseRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnChaseRangeEndOverlap);

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeOverlap);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::OnAttackRangeEndOverlap);
	
	DamageCollision1->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::OnDamageCollisionOverlap);

	OnTakePointDamage.AddDynamic(this, &AEnemyCharacter::TakePointDamage);

}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateToTarget(DeltaTime);

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

void AEnemyCharacter::OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetInChaseRange"), true);
		AIController->GetBlackBoard()->SetValueAsVector(FName("DetectedLocation"), OtherActor->GetActorLocation());
	}
}

void AEnemyCharacter::OnChaseRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* Cha = Cast<AAOSCharacter>(OtherActor);
	if (Cha && AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetInChaseRange"), false);
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
	if (PlayerCharacter && IsAttacking)
	{
		UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, GetController(), this, UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("%s hits %s with a %s"), *GetName(), *PlayerCharacter->GetName(), *OverlappedComponent->GetName());
	}
}

void AEnemyCharacter::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (AIController)
	{
		AIController->GetBlackBoard()->SetValueAsBool(FName("TargetHitsMe"), true);

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
				PlayHitReactMontage();
			}
			else
			{
				if (EnemyState == EEnemyState::EES_Patrol)
				{
					SetEnemyState(EEnemyState::EES_Detected);
				}
			}
		}

		AIController->GetBlackBoard()->SetValueAsVector(FName("DetectedLocation"), DamageCauser->GetActorLocation());
	}

	HandleHealthChange(DamageReceived);

	PopupDamageAmountWidget(InstigatedBy, HitLocation, DamageReceived, BoneName);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), DamageReceived);
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
		// ÀÌÆåÆ® Àç»ý
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
		UE_LOG(LogTemp, Warning, TEXT("pop"));
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
	IsAttacking = true;
	OnAttackEnd.Broadcast();
	PlayAttackMontage();
}

void AEnemyCharacter::PlayAttackMontage()
{
	if (EnemyAnim == nullptr || AttackMontage == nullptr) return;

	int8 RandSectionNum = UKismetMathLibrary::RandomInteger(AttackMontageSectionNameArr.Num());

	EnemyAnim->Montage_Play(AttackMontage);

	EnemyAnim->Montage_JumpToSection(AttackMontageSectionNameArr[RandSectionNum]);
}

void AEnemyCharacter::PlayHitReactMontage()
{
	if (EnemyAnim == nullptr || HitReactMontage == nullptr) return;

	EnemyAnim->Montage_Play(HitReactMontage);

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
		IsAttacking = false;
		OnAttackEnd.Broadcast();
	}
}

void AEnemyCharacter::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactMontage && AIController)
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
	if (EnemyState == EEnemyState::EES_Chase &&
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

		float RotateRate = IsAttacking ? 10.f : 20.f;

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


void AEnemyCharacter::SetEnemyState(EEnemyState State)
{
	switch (State)
	{
	case EEnemyState::EES_Patrol:
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		EnemyStateBefore = State;
		break;
	case EEnemyState::EES_Detected:
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		break;
	case EEnemyState::EES_Comeback:
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		break;
	case EEnemyState::EES_Chase:
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		EnemyStateBefore = State;
		break;
	}

	EnemyState = State;
}

float AEnemyCharacter::GetAcceptableRaius() const
{
	return AcceptableRadius;
}

EEnemyState AEnemyCharacter::GetEnemyState() const
{
	return EnemyState;
}

