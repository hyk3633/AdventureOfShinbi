
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

DECLARE_DELEGATE(OnAttackButtonPressedDelegate);
DECLARE_DELEGATE(OnLevelTransitionDelegate);
DECLARE_DELEGATE_OneParam(OnAimButtonPressedDelegate, bool bPress);

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Nothing,
	ECS_AnimationPlaying,
	ECS_Reloading,
	ECS_Freezed,
	ECS_Stiffed,
	ECS_Dead,

	ECS_MAX
};

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

	void RestartPlayerCharacter();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

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

	FName DistinguishHitDirection(FVector DamageCauserLocation);

	void PlayerKnockBack(AActor* DamageCauser, float Power);

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

	void SetOverlappingItem();

	void TraceItem(FHitResult& HitItem);

private:

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
	void Skill3ButtonPressed();

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationStart();

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationEnd();

	void Fire();
	void StartTimerFire();
	void FireReturn();

	void PlayDashMotage();

	UFUNCTION(BlueprintCallable)
	void OnDashMontageEnded();

	void PlayHitEffect(FVector HitLocation, FRotator HitRotation);

	void InventoryAnimationEnd();

	UFUNCTION(BlueprintCallable)
	void ReloadingStart();

	UFUNCTION(BlueprintCallable)
	void ReloadingEnd();

	void PlayerRespawn();

	void CameraSaturaion(float DeltaTime);

private:

	AAOSController* CharacterController;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UAOSAnimInstance* AnimInstance;

	int32 OverlappingItemCount = 0;

	bool bExistOverlappingItem = false;

	bool bIsRunning = false;

	bool bCanRunning = true;

	bool bIsMoving = false;

	bool bIsAiming = false;

	bool bAttackButtonPressing = false;

	bool bAbleAttack = false;

	bool bAbleFire = true;

	bool bIsInventoryOn = false;

	bool bInventoryAnimationPlaying = false;
	FTimerHandle InventoryAnimationTimer;

	FTimerHandle PlayerRespawnTimer;
	float PlayerRespawnTime = 5.5f;

	bool bDashing = false;

	AItem* OverlappingItem;

	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Character | Info")
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	UPROPERTY(VisibleAnywhere)
	UItemComponent* ItemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GunRecoil = 0.f;

	FTimerHandle FireTimer;

	AItem* OverlappingItemLastFrame = nullptr;

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

	bool bOverlappedLTV = false;

	bool bCameraSaturationOn = false;
	float InitialSaturation = 1.f;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> FreezingSignClass;

	UUserWidget* FreezingSign;

	UPROPERTY(EditAnywhere, Category = "Character | Sign")
	TSubclassOf<UUserWidget> SlowSignClass;

	UUserWidget* SlowSign;
	
public:

	UCameraComponent* GetCamera() const;
	USpringArmComponent* GetSpringArm() const;
	UAOSAnimInstance* GetAnimInst() const;
	void SetCharacterState(ECharacterState State);
	bool GetIsRunning() const;
	void SetCanRunning(bool IsCapable);
	bool GetIsAnimationPlaying() const;
	bool GetIsMoving() const;
	bool GetIsAiming() const;
	void DeactivateAiming();
	bool GetAttackButtonPressing() const;
	void CallAttackFunction();
	void SetAbleAttackFalse();
	void SetOverlappingItemCount(int8 Quantity);
	void ResumeRunning();
	void StopRunning();
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
};
