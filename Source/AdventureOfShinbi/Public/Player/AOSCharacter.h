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

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Nothing UMETA(DisplayName = "Nothing"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),

	ECT_MAX UMETA(DisplayName = "DefaultMAX")
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

	TArray<FColor> color;
	TArray<float> AngleOffset;
	TArray<float> AngleAxis;
	FVector AxisVector;

	FVector Right;
	FVector Forward;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UAOSAnimInstance* AnimInstance;

	int32 OverlappingItemCount = 0;

	bool bExistOverlappingItem = false;

	bool bIsRunning = false;

	bool bCanRunning = true;

	bool bIsAnimationPlaying = false;

	bool bIsMoving = false;

	bool bIsAiming = false;

	bool bAttackButtonPressing = false;

	bool bAbleFire = true;

	bool bIsInventoryOn = false;

	bool bDashing = false;

	AItem* OverlappingItem;

	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_None;

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	UPROPERTY(VisibleAnywhere)
	UItemComponent* ItemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GunRecoil = 0.f;

	FTimerHandle FireTimer;

	AItem* OverlappingItemLastFrame = nullptr;

	UPROPERTY(EditAnywhere)
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere)
	float DashPower = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Effect")
	USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, Category = "Moving Speed")
	float CurrentRunningSpeed = 1000.f; // 600
	UPROPERTY(VisibleAnywhere, Category = "Moving Speed")
	float CurrentWalkingSpeed = 350.f;

	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float OriginRunningSpeed = 1000.f; // 600
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float OriginWalkingSpeed = 350.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float OriginCrouchedSpeed = 250.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float ArmedRunningSpeed = 700.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float ArmedWalkingSpeed = 550.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float SlowedRunningSpeed = 250.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float SlowedWalkingSpeed = 150.f;
	UPROPERTY(EditAnywhere, Category = "Moving Speed")
	float SlowedCroucedSpeed = 100.f;

	bool bIsFreezed = false;
	
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
