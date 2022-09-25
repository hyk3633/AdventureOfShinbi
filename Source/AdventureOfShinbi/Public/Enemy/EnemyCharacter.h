
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
class UParticleSystem;
class USoundCue;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{   
	EES_Patrol UMETA(DisplayName = "Patrol"),
	EES_Detected UMETA(DisplayName = "Detected"),
	EES_Comeback UMETA(DisplayName = "Comeback"),
	EES_Chase UMETA(DisplayName = "Chase"),
	EES_Siege UMETA(DisplayName = "Siege"),

	EES_MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	void Attack();

	FOnAttackEndDelegate OnAttackEnd;

	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void CheckIsKnockUp();

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

	virtual void HandleStiffAndStun(FName& BoneName);

	void HandleHealthChange(float DamageReceived);

	void PopupDamageAmountWidget(AController* InstigatorController, FVector PopupLocation, float DamageNumber, FName HittedBoneName);

	virtual void PlayAttackMontage();

	void PlayHitReactionMontage();

	void PlayStunMontage();

	void PlayDeathMontage();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void DeathMontageEnded();

	void ForgetHit();

	void RotateToTarget(float DeltaTime);

	void SetHealthBar();

	UFUNCTION(BlueprintCallable)
	void ActivateDamageCollision1();

	UFUNCTION(BlueprintCallable)
	void DeactivateDamageCollision1();

protected:

	AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Damage;

	UPROPERTY()
	UEnemyAnimInstance* EnemyAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bIsAttacking;

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrol;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float StiffChance = 0.35f;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float StunChance = 0.2f;

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* AttackRange;

	UPROPERTY(EditAnywhere)
	UBoxComponent* DamageCollision1;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> AttackMontageSectionNameArr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Defense;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float PatrolSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float ChaseSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float AcceptableRadius = 10.f;

	FTimerHandle HitForgetTimer;

	UPROPERTY(EditAnywhere, Category = "AI Settings")
	float HitMemoryTime = 10.f;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthWidget;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> DamageAmountTextClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* DeathSound;

public:

	float GetAcceptableRaius() const;
	EEnemyState GetEnemyState() const;
	void SetEnemyState(EEnemyState State);
	bool GetIsAttacking() const;

	UFUNCTION(BlueprintCallable)
	FVector GetPatrolPoint() const;

};
