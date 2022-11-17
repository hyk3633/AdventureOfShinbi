// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types/WeaponType.h"
#include "AOSCharacter.generated.h"

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
DECLARE_DELEGATE_OneParam(OnAimButtonPressedDelegate, bool bPress);

UENUM()
enum class ECharacterState : uint8
{
	ECS_Nothing,
	ECS_AnimationPlaying,
	ECS_Freezed,
	ECS_Stiffed,

	ECS_MAX
};

enum class EWalkingState : uint8
{
	EWS_UnArmed,
	EWS_Armed,
	EWS_Slowed,

	EWS_MAX
};

UCLASS()
class ADVENTUREOFSHINBI_API AAOSCharacter : public ACharacter
{
	GENERATED_BODY()
public:

	AAOSCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	OnAttackButtonPressedDelegate DAttackButtonPressed;

	OnAimButtonPressedDelegate DAimButtonPressed;

protected:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

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

	void PlayerKnockBack(AActor* DamageCauser, float Power);

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

private:

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

	bool bAbleFire = true;

	bool bIsInventoryOn = false;

	bool bDashing = false;

	AItem* OverlappingItem;

	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Character | Info")
	EWeaponType WeaponType = EWeaponType::EWT_None;

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
	USoundCue* HitSound;

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
	
public:

	UCameraComponent* GetCamera() const;
	USpringArmComponent* GetSpringArm() const;
	bool GetIsRunning() const;
	void SetCanRunning(bool IsCapable);
	bool GetIsAnimationPlaying() const;
	bool GetIsMoving() const;
	bool GetIsAiming() const;
	void DeactivateAiming();
	bool GetAttackButtonPressing() const;
	void SetOverlappingItemCount(int8 Quantity);
	EWeaponType GetWeaponType() const;
	void SetWeaponType(EWeaponType Type);
	void ResumeRunning();
	void StopRunning();
	void SetGunRecoil(float Recoil);
	UCombatComponent* GetCombatComp() const;
	UItemComponent* GetItemComp() const;
	void SetWalkingSpeed(EWalkingState State);
	void ActivateFreezing(bool IsActivate);
};
