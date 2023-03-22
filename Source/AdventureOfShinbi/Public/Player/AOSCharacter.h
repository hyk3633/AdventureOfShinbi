
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

/** ĳ���� ���� enum */
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

/** ĳ���� �̵� �ӵ� ���� enum */
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

	/** �÷��̾��� ����, ������ ���� �缳�� */
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
	* �ǰ� ��ġ �Ǻ� 
	* @param TargetLocation �������� ���� ����� ��ġ
	*/
	FName DistinguishDirection(FVector TargetLocation);

	/** �÷��̾ �ڷ� �з������� */
	void PlayerKnockBack(AActor* DamageCauser, float Power);

	/** ���̽� ��� �� ��Ʈ ���׼� �Լ� ȣ�� */
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

	/** ���� �÷��̾ �� �ִ� �ݰ��� �������� ���� */
	void SetOverlappingItem();

	/** �ݰ濡 �� �ִ� �������� Ʈ���̽��� ���� */
	void TraceItem(FHitResult& HitItem);

private:

	/** �÷��̾� ���� ó�� */
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

	/** ���� �� Ÿ���� Ž�� */
	AActor* FindTarget();

	/** Ÿ���� ���ϴ� Rotation ��� */
	FRotator CalculateRotation(AActor* Target);

	/** �ִϸ��̼� ��� ������ �÷��̾� ���� enum ���� */
	UFUNCTION(BlueprintCallable)
	void TransitionAnimationStart();

	/** �⺻ ���� enum ���� */
	UFUNCTION(BlueprintCallable)
	void TransitionAnimationEnd();

	UFUNCTION(BlueprintCallable)
	void HitReactionAnimationStart();

	/** CombatComponent�� GunFire ȣ�� �� �߻� ���� Ÿ�̸� ���� */
	void Fire();
	/** �߻� ���� Ÿ�̸� */
	void StartTimerFire();

	/** Ÿ�̸� ���� �� �ٽ� �߻� �Լ� ȣ�� */
	void FireReturn();

	void PlayDashMotage();

	UFUNCTION()
	void DashMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

	UFUNCTION(BlueprintCallable)
	void ReloadingStart();

	UFUNCTION(BlueprintCallable)
	void ReloadingEnd();

	/** �÷��̾� ������ �Լ� ȣ�� */
	void PlayerRespawn();

	/** ī�޶� ä���� ������� */
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

	/** �ݰ濡 ���� �������� �ִ��� ���� */
	bool bExistOverlappingItem = false;

	/** �޸��� �� ���� */
	bool bIsRunning = false;

	/** �޸��� ���� ���� */
	bool bCanRunning = true;

	/** ���� �� ���� */
	bool bIsAiming = false;

	/** ���� ��ư�� Ŭ�� �� ���� */
	bool bAttackButtonPressing = false;

	/** ���� ���� �������� */
	bool bAbleAttack = false;

	/** �߻� ���� Ÿ�̸Ӱ� ����Ǹ� true */
	bool bAbleFire = true;

	/** �κ��丮�� Ȱ��ȭ �� ���� */
	bool bIsInventoryOn = false;

	/** �÷��̾� ������ ������ Ÿ�̸� */
	FTimerHandle PlayerRespawnTimer;
	float PlayerRespawnTime = 5.5f;

	/** �ݰ� ���� ���� ������ */
	UPROPERTY()
	AItem* OverlappingItem;

	/** ĳ���� ���� */
	UPROPERTY(VisibleAnywhere, Category = "Character | Info")
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	/** ���� �ִϸ��̼�, ĳ���� ����, ���� ȹ��/���� ���� ����� �Ѱ��ϴ� ���� ������Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	/** ������ ȹ�� �� ���, ü�� ȸ��, ���� ȸ�� ���� ����� �Ѱ��ϴ� ���� ������Ʈ */
	UPROPERTY(VisibleAnywhere)
	UItemComponent* ItemComp;

	/** ���� ������ ������ �ݵ� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GunRecoil = 0.f;

	/** �߻� ���� Ÿ�̸� */
	FTimerHandle FireTimer;

	/** ������ �ݰ� �� ������ */
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

	/** ���� Ʈ������ ������ ������ ���ִ��� ���� */
	bool bOverlappedLTV = false;

	/** ī�޶� ä�� ��ȯ ���� */
	bool bCameraSaturationOn = false;

	/** ī�޶� ä�� �ʱⰪ */
	float InitialSaturation = 1.f;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> FreezingSignClass;

	UPROPERTY()
	UUserWidget* FreezingSign;

	/** ������ ����¡ ��ų �ǰ� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bFreezed = false;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> SlowSignClass;

	UPROPERTY()
	UUserWidget* SlowSign;

	/** �̵� �ӵ� ���� ���� */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bSlowed = false;

	/** ī�޶� ���� ���� */
	bool bCameraLockOn = false;

	/** ���� �� �� ĳ���� */
	UPROPERTY()
	AEnemyCharacter* LockOnTarget;

	UPROPERTY(EditAnywhere, Category = "Character | LockOn")
	UParticleSystem* LockOnParticle;

	UPROPERTY()
	UParticleSystemComponent* LockOnParticleComp;

	/** ���� Ÿ�̸� */
	FTimerHandle StiffTimer;

	/** �̵� �ӵ� ���� Ÿ�̸� */
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
