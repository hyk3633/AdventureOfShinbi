
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/TimelineComponent.h"
#include "EnemyCharacter.generated.h"

class AEnemyAIController;
class USphereComponent;
class UEnemyAnimInstance;
class UAnimMontage;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UWidgetComponent;
class UParticleSystem;
class UParticleSystemComponent;
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

USTRUCT(Atomic)
struct FAiInfo 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector DetectedLocation = FVector::ZeroVector;

	UPROPERTY()
	AActor* TargetPlayer = nullptr;
	UPROPERTY()
	bool bTargetIsVisible = false;
	UPROPERTY()
	bool bTargetIsHeard = false;
	UPROPERTY()
	bool bSightStimulusExpired = true;

	UPROPERTY()
	bool bIsKnockUp = false;
	UPROPERTY()
	bool bStunned = false;
	UPROPERTY()
	bool bStiffed = false;
	UPROPERTY()
	bool bTargetInAttackRange = false;
	UPROPERTY()
	bool bTargetHitsMe = false;

	UPROPERTY()
	bool bIsPlayerDead = false;
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	virtual void Attack();

	FOnAttackEndDelegate OnAttackEnd;

	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

	void PlayMeleeAttackEffect(FVector HitLocation, FRotator HitRotation);

	void ActivateHealing(float RecoveryAmount);

	void ActivateDamageUp(float DamageUpRate);

	void CheckNothingStimulus();

	UFUNCTION(BlueprintCallable)
	void SetStateToPatrol();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual bool Weapon1BoxTrace();

	void GetBoxTraceHitResult(FHitResult& HitResult, FName StartSocketName, FName EndSocketName);

	void CheckIsKnockUp();

	void EndKnockUpDelay();

	virtual void AbortAttack();

	void Healing(float DeltaTime);

	UFUNCTION()
	void OnDetected(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	virtual void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAttackRangeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void TakePointDamage
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

	FName DistinguishHitDirection();

	void PlayStunMontage();

	virtual void PlayDeathMontage();

	void Dissolution();

	UFUNCTION()
	void UpdateDissolveMat(float Value);

	UFUNCTION()
	virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void EndStiffDelay();

	UFUNCTION()
	void OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	virtual void DeathMontageEnded();

	void DestroyCharacter();

	void ForgetHit();

	virtual void RotateToTarget(float DeltaTime);

	virtual bool CheckRotateToTargetCondition();

	void SetHealthBar();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponTrace1();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponTrace1();

	UFUNCTION(BlueprintCallable)
	virtual void StopAttackMontage();

	void DamageUpTimeEnd();

	void PlayBuffParticle();

	void SightStimulusExpire();

	USphereComponent* GetAttackRange() const;

protected:

	AEnemyAIController* AIController;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UEnemyAnimInstance* EnemyAnim;

	FAiInfo AiInfo;

	FName WeaponTraceStartSocketName;
	FName WeaponTraceEndSocketName;

	bool bActivateWeaponTrace1 = false;

	bool bDeath = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy | Stats", Meta = (AllowPrivateAccess = true))
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, Category = "Enemy | Stats")
	EEnemyState EnemyState = EEnemyState::EES_Patrol;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float PatrolSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float ChaseSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	FVector BoxTraceSize;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float StiffChance = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float StunChance = 0.2f;

private:

	UAISenseConfig_Sight* SightConfig;

	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditAnywhere)
	USphereComponent* AttackRange;

	float AcceptableRadius;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	TArray<FName> AttackMontageSectionNameArr;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	UParticleSystem* MeleeHitParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	USoundCue* MeleeHitSound;

	UPROPERTY(EditAnywhere, Category = "Enemy | Hit")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Hit")
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Hit")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Hit")
	USoundCue* HitVoice;

	FTimerHandle HitForgetTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	UCurveFloat* DissolveCurve;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	float DestroyTime = 3.f;

	UPROPERTY(VisibleAnywhere, Category = "Enemy | Dissolve")
	TArray<UMaterialInstanceDynamic*> DynamicDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	TArray<UMaterialInstance*> DissolveMatInst;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy | Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy | Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float Defense;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float HitMemoryTime = 10.f;

	FTimerHandle SightStimulusExpireTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float SightStimulusExpireTime = 10.f;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthWidget;

	UPROPERTY(EditAnywhere, Category = "Enemy | HUD")
	TSubclassOf<UUserWidget> DamageAmountTextClass;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	bool bHealing = false;

	float HealAmount = 0.f;

	float HealedAmount = 0.f;

	float RecoveryRate = 10.f;

	UPROPERTY(VisibleAnywhere)
	float OriginDamage = 0.f;

	FTimerHandle DamageUpTimer;

	float DamageUpTime = 30.f;

	bool IsPlayerDeathDelegateBined = false;

	FTimerHandle StiffDelayTimer;

	FTimerHandle KnockUpDelayTimer;

public:

	float GetAcceptableRaius() const;
	EEnemyState GetEnemyState() const;
	void SetEnemyState(EEnemyState State);
	bool GetIsAttacking() const;
	bool GetIsDead() const;
	void SetDamage(float DamageUpRate = 0.f);

	UFUNCTION(BlueprintCallable)
	FVector GetPatrolPoint() const;

	float GetHealthPercentage() const;
	float GetMaxHealth() const;
	float GetEnemyDamage() const;

	FAiInfo GetAiInfo() const;
	FVector GetPatrolPoint();
};
