
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types/WeaponType.h"
#include "AOSCharacter.generated.h"

class AAOSController;
class UInputComponent;
class USpringArmComponent;
class UCameraComponent;
class UAOSAnimInstance;
class AWeapon;
class AItem;
class UCombatComponent;
class UItemComponent;
class UAnimMontage;
class UParticleSystem;
class USoundCue;
class AEnemyCharacter;

DECLARE_DELEGATE(OnAttackButtonPressedDelegate);
DECLARE_DELEGATE(OnLevelTransitionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSecretInteractionDelegate);
DECLARE_DELEGATE_OneParam(OnAimButtonPressedDelegate, bool bPress);

/** 캐릭터 상태 enum */
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Nothing ,
	ECS_AnimationPlaying,
	ECS_Reloading,
	ECS_Freezed,
	ECS_Stiffed,
	ECS_Dead,

	ECS_MAX
};

/** 캐릭터 이동 속도 설정 enum */
enum class EWalkingState : uint8
{
	EWS_UnArmed,
	EWS_Armed,
	EWS_Slowed,

	EWS_MAX
};

UCLASS(BlueprintType)
class ADVENTUREOFSHINBI_API AAOSCharacter : public ACharacter
{
	GENERATED_BODY()
public:

	AAOSCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	OnAttackButtonPressedDelegate DAttackButtonPressed;

	OnLevelTransitionDelegate DLevelTransition;

	OnAimButtonPressedDelegate DAimButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FSecretInteractionDelegate DSecretInteraction;

	/** 플레이어의 무기, 아이템 상태 재설정 */
	void RestartPlayerCharacter();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void SetCharacterMesh();

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

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

	/** 
	* 피격 위치 판별 
	* @param TargetLocation 데미지를 입힌 대상의 위치
	*/
	FName DistinguishDirection(FVector TargetLocation);

	/** 플레이어를 뒤로 밀려나게함 */
	void PlayerKnockBack(AActor* DamageCauser, float Power);

	/** 보이스 재생 및 히트 리액션 함수 호출 */
	void PlayHitReaction(USoundCue* Voice, AActor* DamageCauser);

	UFUNCTION()
	void TakeRadialDamage
	(
		AActor* DamagedActor,
		float DamageReceived,
		const UDamageType* DamageType,
		FVector Origin,
		FHitResult HitInfo,
		AController* InstigatedBy,
		AActor* DamageCauser
	);

	/** 현재 플레이어가 들어가 있는 반경의 아이템을 설정 */
	void SetOverlappingItem();

	/** 반경에 들어가 있는 아이템을 트레이스로 추적 */
	void TraceItem(FHitResult& HitItem);

private:

	/** 플레이어 죽음 처리 */
	UFUNCTION()
	void HandlePlayerDeath();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Lookup(float Value);
	void Turn(float Value);
	void Jump();
	void RunningButtonPressed();
	void RunningButtonReleased();
	void Crouch_DashButtonPressed();
	void Crouching();
	void Dash();
	void Equip_Skill1ButtonPressed();
	void AttackButtonPressed();
	void AttackButtonReleassed();
	void AimButtonPressed();
	void AimButtonReleased();
	void Reload_Skill2ButtonPressed();
	void InventoryKeyPressed();
	void WeaponQuickSwapKeyPressed();
	void UseItemKeyPressed();
	void ItemChangeKeyPressed();
	void MouseWheelButtonPressed();

	void SetLockOnTarget();

	void UpdateControlRotation();

	void DeactivateLockOn();

	/** 락온 할 타겟을 탐색 */
	AActor* FindTarget();

	/** 타겟을 향하는 Rotation 계산 */
	FRotator CalculateRotation(AActor* Target);

	/** 애니메이션 재생 중으로 플레이어 상태 enum 설정 */
	UFUNCTION(BlueprintCallable)
	void TransitionAnimationStart();

	/** 기본 상태 enum 설정 */
	UFUNCTION(BlueprintCallable)
	void TransitionAnimationEnd();

	UFUNCTION(BlueprintCallable)
	void HitReactionAnimationStart();

	/** CombatComponent의 GunFire 호출 및 발사 지연 타이머 시작 */
	void Fire();
	/** 발사 지연 타이머 */
	void StartTimerFire();

	/** 타이머 종료 후 다시 발사 함수 호출 */
	void FireReturn();

	void PlayDashMotage();

	UFUNCTION()
	void DashMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

	UFUNCTION(BlueprintCallable)
	void ReloadingStart();

	UFUNCTION(BlueprintCallable)
	void ReloadingEnd();

	/** 플레이어 리스폰 함수 호출 */
	void PlayerRespawn();

	/** 카메라 채도를 흑백으로 */
	void CameraSaturation(float DeltaTime);

	void SlowEnd();

private:

	UPROPERTY()
	AAOSController* CharacterController;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY()
	UAOSAnimInstance* AnimInstance;

	int32 OverlappingItemCount = 0;

	FSoftObjectPath RefPath;

	/** 반경에 들어온 아이템이 있는지 여부 */
	bool bExistOverlappingItem = false;

	/** 달리기 중 인지 */
	bool bIsRunning = false;

	/** 달리기 가능 여부 */
	bool bCanRunning = true;

	/** 조준 중 인지 */
	bool bIsAiming = false;

	/** 공격 버튼이 클릭 중 인지 */
	bool bAttackButtonPressing = false;

	/** 공격 가능 상태인지 */
	bool bAbleAttack = false;

	/** 발사 지연 타이머가 종료되면 true */
	bool bAbleFire = true;

	/** 인벤토리가 활성화 중 인지 */
	bool bIsInventoryOn = false;

	/** 플레이어 리스폰 딜레이 타이머 */
	FTimerHandle PlayerRespawnTimer;
	float PlayerRespawnTime = 5.5f;

	/** 반경 내에 들어온 아이템 */
	UPROPERTY()
	AItem* OverlappingItem;

	/** 캐릭터 상태 */
	UPROPERTY(VisibleAnywhere, Category = "Character | Info")
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	/** 전투 애니메이션, 캐릭터 스탯, 무기 획득/공격 등의 기능을 총괄하는 액터 컴포넌트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	/** 아이템 획득 및 사용, 체력 회복, 마나 회복 등의 기능을 총괄하는 액터 컴포넌트 */
	UPROPERTY(VisibleAnywhere)
	UItemComponent* ItemComp;

	/** 현재 장착한 무기의 반동 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GunRecoil = 0.f;

	/** 발사 지연 타이머 */
	FTimerHandle FireTimer;

	/** 이전의 반경 내 아이템 */
	UPROPERTY()
	AItem* OverlappedItemPrev = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character | Dash")
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, Category = "Character | Dash")
	float DashPower = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Character | Effect")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Effect")
	UParticleSystem* RespawnParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Effect")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, Category = "Character | Effect")
	USoundCue* RespawnSound;

	UPROPERTY(EditAnywhere, Category = "Character | Voice")
	USoundCue* VoiceJump;

	UPROPERTY(EditAnywhere, Category = "Character | Voice")
	USoundCue* VoicePain;

	UPROPERTY(EditAnywhere, Category = "Character | Voice")
	USoundCue* VoicePainHeavy;

	UPROPERTY(EditAnywhere, Category = "Character | Effect")
	TSubclassOf<UCameraShakeBase> CameraShakeHitted;

	UPROPERTY(VisibleAnywhere, Category = "Character | Moving Speed")
	float CurrentRunningSpeed = 600.f;
	UPROPERTY(VisibleAnywhere, Category = "Character | Moving Speed")
	float CurrentWalkingSpeed = 350.f;

	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float OriginRunningSpeed = 600.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float OriginWalkingSpeed = 350.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float OriginCrouchedSpeed = 250.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float ArmedRunningSpeed = 700.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float ArmedWalkingSpeed = 550.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float SlowedRunningSpeed = 250.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float SlowedWalkingSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "Character | Moving Speed")
	float SlowedCroucedSpeed = 100.f;

	/** 레벨 트랜지션 볼륨과 오버랩 되있는지 여부 */
	bool bOverlappedLTV = false;

	/** 카메라 채도 변환 여부 */
	bool bCameraSaturationOn = false;

	/** 카메라 채도 초기값 */
	float InitialSaturation = 1.f;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> FreezingSignClass;

	UPROPERTY()
	UUserWidget* FreezingSign;

	/** 보스의 프리징 스킬 피격 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bFreezed = false;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> SlowSignClass;

	UPROPERTY()
	UUserWidget* SlowSign;

	/** 이동 속도 감소 여부 */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSlowed = false;

	/** 카메라 락온 여부 */
	bool bCameraLockOn = false;

	/** 락온 한 적 캐릭터 */
	UPROPERTY()
	AEnemyCharacter* LockOnTarget;

	UPROPERTY(EditAnywhere, Category = "Character | LockOn")
	UParticleSystem* LockOnParticle;

	UPROPERTY()
	UParticleSystemComponent* LockOnParticleComp;

	/** 경직 타이머 */
	FTimerHandle StiffTimer;

	/** 이동 속도 감소 타이머 */
	FTimerHandle SlowTimer;
	
public:

	UCameraComponent* GetCamera() const;
	USpringArmComponent* GetSpringArm() const;
	UAOSAnimInstance* GetAnimInst() const;
	void SetCharacterState(ECharacterState State);
	bool GetIsRunning() const;
	void StopRunning();
	void StartRunning();
	bool GetIsAnimationPlaying() const;
	bool GetIsAiming() const;
	void DeactivateAiming();
	bool GetAttackButtonPressing() const;
	void CallAttackFunction();
	void SetAbleAttackFalse();
	void SetOverlappingItemCount(int8 Quantity);
	void SetGunRecoil(float Recoil);
	UCombatComponent* GetCombatComp() const;
	UItemComponent* GetItemComp() const;
	void SetWalkingSpeed(EWalkingState State);
	void ActivateFreezing(bool IsActivate);
	void SetView(EWeaponType Type);
	void ActivateWeaponControlMode();
	void DeactivateWeaponControlMode();
	void SetOverlappedLTV(bool bIsOverlap);
	void SetCharacterController();
	void WeaponStateChanged(AWeapon* Weapon);
};
