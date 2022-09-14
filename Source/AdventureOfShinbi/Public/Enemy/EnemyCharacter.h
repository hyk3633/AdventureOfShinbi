
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class AEnemyAIController;
class USphereComponent;
class UBoxComponent;
class UEnemyAnimInstance;
class UAnimMontage;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UENUM()
enum class EEnemyState : uint8
{   
	EES_Patrol,
	EES_Detected,
	EES_Comeback,
	EES_Chase,

	EES_MAX
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	virtual void Attack();

	FOnAttackEndDelegate OnAttackEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnChaseRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnDamageCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TakePointDamage
	(
		AActor* DamagedActor, 
		float DamageReceived,
		class AController* InstigatedBy,
		FVector HitLocation,
		class UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const class UDamageType* DamageType,
		AActor* DamageCauser
	);

	void HandleHealthChange(float DamageReceived);

	void PopupDamageAmountWidget(AController* InstigatorController, FVector PopupLocation, float DamageNumber, FName HittedBoneName);

	virtual void PlayAttackMontage();

	void PlayHitReactMontage();

	void PlayStunMontage();

	void PlayDeathMontage();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ForgetHit();

	void RotateToTarget(float DeltaTime);

	void SetHealthBar();

protected:

	AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Damage;

	UPROPERTY()
	UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrol;

	EEnemyState EnemyStateBefore = EEnemyState::EES_Patrol;

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* ChaseRange;

	UPROPERTY(EditAnywhere)
	USphereComponent* AttackRange;

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision1;

	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere)
	TArray<FName> AttackMontageSectionNameArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Defense;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float MovingSpeed;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float PatrolSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float ChaseSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float AcceptableRadius = 10.f;

	FTimerHandle HitForgetTimer;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float HitMemoryTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float StiffChance = 0.35f;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float StunChance = 0.2f;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthWidget;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> DamageAmountTextClass;

public:

	float GetAcceptableRaius() const;
	EEnemyState GetEnemyState() const;
	void SetEnemyState(EEnemyState State);
};
