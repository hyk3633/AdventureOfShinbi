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

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationStart();

	UFUNCTION(BlueprintCallable)
	void TransitionAnimationEnd();

private:

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	bool bIsRunning = false;

	bool bIsAnimationPlaying = false;

	bool bIsMoving = false;

	bool bIsAiming = false;

	AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_MAX;

	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterState = ECharacterState::ECS_Nothing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

public:

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool GetIsRunning() const { return bIsRunning; }
	FORCEINLINE bool GetIsAnimationPlaying() const { return bIsAnimationPlaying; }
	FORCEINLINE bool GetIsMoving() const { return bIsMoving; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE EWeaponType GetCombatState() const { return WeaponType; }

};
