// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types/WeaponType.h"
#include "AOSCharacter.generated.h"

class UInputComponent;
class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class UCombatComponent;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Nothing UMETA(DisplayName = "Nothing"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),

	ECT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class ADVENTUREOFSHINBI_API AAOSCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AAOSCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetOverlappingWeapon(AWeapon* OtherWeapon);

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

private:

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Lookup(float Value);
	void Turn(float Value);
	void Jump();
	void RunningButtonPressed();
	void RunningButtonReleased();
	void CrouchButtonPressed();
	void EquipButtonPressed();
	void AttackButtonePressed();
	void AttackButtoneReleassed();
	void AimButtonPressed();
	void AimButtonReleased();
	void ReloadButtonPressed();
	void InventoryKeyPressed();
	void WeaponQuickSwapKeyPressed();

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationStart();

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationEnd();

	void Fire();
	void StartTimerFire();
	void FireReturn();


private:

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	bool bIsRunning = false;

	bool bCanRunning = true;

	bool bIsAnimationPlaying = false;

	bool bIsMoving = false;

	bool bIsAiming = false;

	bool bAttackButtonPressing = false;

	bool bAbleFire = true;

	bool bIsInventoryOn = false;

	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_None;

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float GunRecoil = 0.f;

	FTimerHandle FireTimer;

public:

	UCameraComponent* GetCamera() const;
	bool GetIsRunning() const;
	void SetCanRunning(bool IsCapable);
	bool GetIsAnimationPlaying() const;
	bool GetIsMoving() const;
	bool GetIsAiming() const;
	bool GetAttackButtonPressing() const;
	EWeaponType GetWeaponType() const;
	void SetWeaponType(EWeaponType Type);
	void ResumeRunning();
	void StopRunning();
	void SetGunRecoil(float Recoil);

};
