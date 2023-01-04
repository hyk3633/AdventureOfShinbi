
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

/**
* 적 캐릭터의 기본 클래스
*/

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

/** 적 캐릭터 움직임 상태 enum */
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

/** 적 캐릭터의 AI 상태 정보 구조체 */
USTRUCT(Atomic)
struct FAiInfo 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector DetectedLocation = FVector::ZeroVector; // 플레이어의 공격 또는 노이즈로 인해 감지된 위치

	UPROPERTY()
	AActor* TargetPlayer = nullptr; // 타겟 플레이어
	UPROPERTY()
	bool bTargetIsVisible = false; // 타겟이 보이면 true
	UPROPERTY()
	bool bTargetIsHeard = false; // 타겟의 소리가 들리면 true
	UPROPERTY()
	bool bSightStimulusExpired = true; // 시각 자극이 만료되면 true

	UPROPERTY()
	bool bIsKnockUp = false; // 공중에 떠 있으면 true
	UPROPERTY()
	bool bStunned = false; // 스턴 중 이면 true
	UPROPERTY()
	bool bStiffed = false; // 경직 중 이면 true
	UPROPERTY()
	bool bTargetInAttackRange = false; // 타겟이 공격 범위 내에 있으면 true
	UPROPERTY()
	bool bTargetHitsMe = false; // 타겟에게 공격당했으면 true

	UPROPERTY()
	bool bIsPlayerDead = false; // 플레이어의 사망 여부
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	/** BT_Task가 호출하는 공격 함수 */
	virtual void Attack();

	/** 피격 이펙트 재생 */
	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

	/** 근접 공격 이펙트 재생 */
	void PlayMeleeAttackEffect(FVector HitLocation, FRotator HitRotation);

	/** 체력 재생 버프 함수 */
	void ActivateHealing(float RecoveryAmount);

	/** 공격력 증가 버프 함수 */
	void ActivateDamageUp(float DamageUpRate);

	/** 시각, 청각, 피격, 시각 기억의 모든 자극이 비활성화인지 검사 */
	void CheckNothingStimulus();

	/** 움직임 상태 enum을 partol로 설정 */
	UFUNCTION(BlueprintCallable)
	void SetStateToPatrol();

	void PlayBuffParticle();

	void BuffEnd();

	FOnAttackEndDelegate OnAttackEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual bool Weapon1BoxTrace();

	/** 근접 공격을 위한 박스 트레이스 수행 */
	void GetBoxTraceHitResult(FHitResult& HitResult, FName StartSocketName, FName EndSocketName);

	/** 공중에 떠 있는지 체크 */
	void CheckIsKnockUp();

	/** 착지 후 잠깐의 지연 시간 종료 */
	void EndKnockUpDelay();

	/** 공격 중단 */
	virtual void AbortAttack();

	/** 체력 회복 */
	void Healing(float DeltaTime);

	/** 자극 정보가 갱신될 때 마다 자동으로 호출되는 함수 */
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

	/** 스턴 및 경직 처리 */
	virtual void HandleStiffAndStun(bool IsHeadShot);

	/** 체력 변화 처리 */
	virtual void HandleHealthChange(float DamageReceived);

	/** 데미지 수치 위젯 생성 */
	void PopupDamageAmountWidget(AController* InstigatorController, FVector PopupLocation, float DamageNumber, bool IsHeadShot, bool IsCritical);

	/** AI 정보 초기화 */
	UFUNCTION()
	virtual void ResetAIState();

	virtual void PlayAttackMontage();

	void PlayHitReactionMontage();

	/** 피격 방향 판별 */
	FName DistinguishHitDirection();

	void PlayStunMontage();

	virtual void PlayDeathMontage();

	/** 캐릭터 사망 후 소멸 효과 */
	void Dissolution();

	/** 
	* 커브 값에 따라 소멸 효과 머티리얼의 파라미터 값 설정 
	* @param Value 재생 중인 커브의 현재 커브 값
	*/
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

	/** 피격 자극 만료 */
	void ForgetHit();

	/** 캐릭터를 타겟 방향으로 회전 */
	virtual void RotateToTarget(float DeltaTime);

	/** 캐릭터가 회전 가능한지 검사 */
	virtual bool CheckRotateToTargetCondition();

	virtual void SetHealthBar();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponTrace1();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponTrace1();

	UFUNCTION(BlueprintCallable)
	virtual void StopAttackMontage();

	/** 공격력 버프 종료 */
	void DamageUpTimeEnd();

	/** 시각 자극 만료 */
	void SightStimulusExpire();

	USphereComponent* GetAttackRange() const;

	UFUNCTION()
	virtual void PlayerDead();

	void AttackDelay();

protected:

	AEnemyAIController* AIController;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UEnemyAnimInstance* EnemyAnim;

	FAiInfo AiInfo;

	FName WeaponTraceStartSocketName;
	FName WeaponTraceEndSocketName;

	/** 공격 애니메이션 도중 활성화 되는 bool, true 이면 박스 트레이스가 활성화됨 */
	bool bActivateWeaponTrace1 = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDeath = false;

	/** 공격 중 이면 true */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy | Stats", Meta = (AllowPrivateAccess = true))
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, Category = "Enemy | Stats")
	EEnemyState EnemyState = EEnemyState::EES_Patrol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy | Stats", meta = (AllowPrivateAccess = "true"))
	float Health = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy | Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 800.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats", meta = (ClampMin = "1.0"))
	float Defense = 30.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats", meta = (ClampMin = "1.0"))
	float DefaultValue = 15.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats", meta = (ClampMin = "1"))
	int32 RandRangeValue = 25;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float Damage = 300.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float PatrolSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Stats")
	float ChaseSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Enemy | Attack")
	FVector BoxTraceSize;

	/** 경직 확률 */
	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float StiffChance = 0.35f;

	/** 스턴 확률 */
	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float StunChance = 0.2f;

	/** 시각 자극 만료 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float SightStimulusExpireTime = 10.f;

	/** 공격 도중 캐릭터 회전 속도 */
	float AttackingRotateRate = 15.f;

	/** 캐릭터 회전 속도 */
	float NormalRotateRate = 25.f;

	/** 체력바 위젯 */
	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthWidget;

private:

	UAISenseConfig_Sight* SightConfig;

	UAISenseConfig_Hearing* HearingConfig;

	/** 근접 공격 범위 콜리전 (이 컴포넌트에 타겟이 오버랩되면 근접 공격)  */
	UPROPERTY(EditAnywhere)
	USphereComponent* AttackRange;

	/** 공격 애니메이션이 여러 개인 경우 랜덤 출력을 위한 애니메이션 몽타주의 섹션 이름 배열 */
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

	/** 소멸 효과 커브 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	UCurveFloat* DissolveCurve;

	/** 소멸 효과 타임 라인 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	FTimerHandle DestroyTimer;

	/** 캐릭터 파괴 지연 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	float DestroyTime = 3.f;

	/** 소멸 효과가 적용된 머티리얼 */
	UPROPERTY(VisibleAnywhere, Category = "Enemy | Dissolve")
	TArray<UMaterialInstanceDynamic*> DynamicDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = "Enemy | Dissolve")
	TArray<UMaterialInstance*> DissolveMatInst;

	/** 피격 자극 기억 시간 */
	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	float HitMemoryTime = 10.f;

	FTimerHandle SightStimulusExpireTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | HUD")
	TSubclassOf<UUserWidget> DamageAmountTextClass;

	/** 순찰 위치 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/** 체력 회복 여부 */
	bool bHealing = false;

	float HealAmount = 0.f;

	float HealedAmount = 0.f;

	float RecoveryRate = 100.f;

	/** 데미지 기본 값 */
	UPROPERTY(VisibleAnywhere)
	float OriginDamage = 0.f;

	FTimerHandle DamageUpTimer;

	float DamageUpTime = 30.f;

	/** 플레이어 사망 델리게이트에 함수 바인딩 여부 */
	bool IsPlayerDeathDelegateBined = false;

	FTimerHandle StiffDelayTimer;

	FTimerHandle KnockUpDelayTimer;

	FVector InitialLocation;

	FRotator InitialRotation;

	FTimerHandle AttackDelayTimer;

	UPROPERTY(EditAnywhere, Category = "Enemy | Buff")
	UParticleSystem* BuffStartParticle;

	UPROPERTY(EditAnywhere, Category = "Enemy | Buff")
	UParticleSystem* BuffParticle;

	UParticleSystemComponent* BuffParticleComponent;

public:

	EEnemyState GetEnemyState() const;
	void SetEnemyState(EEnemyState State);
	bool GetIsAttacking() const;
	bool GetIsDead() const;
	void SetDamage(float DamageUpRate = 0.f);

	UFUNCTION(BlueprintCallable)
	FVector GetPatrolPoint() const;

	float GetHealth() const;
	float GetHealthPercentage() const;
	float GetMaxHealth() const;
	virtual float GetEnemyDamage() const;

	FAiInfo GetAiInfo() const;
	FVector GetPatrolPoint();

	float GetDefaultValue() const;
	int32 GetRandRangeValue() const;
};
