// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyStrafing.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnStrafingEndDelegate);

UENUM(BlueprintType)
enum class EStrafingDirection : uint8
{
	ESD_Front UMETA(DisplayName = "Front"),
	ESD_Left UMETA(DisplayName = "Left"),
	ESD_Right UMETA(DisplayName = "Right"),
	ESD_Back UMETA(DisplayName = "Back"),
	ESD_RDiagonal UMETA(DisplayName = "RDiagonal"),
	ESD_LDiagonal UMETA(DisplayName = "LDiagonal"),
	ESD_Stop UMETA(DisplayName = "Stop"),

	ESD_MAX UMETA(Hidden)
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyStrafing : public AEnemyRanged
{
	GENERATED_BODY()
	
public:

	AEnemyStrafing();

	void StartStrafing();

	FOnStrafingEndDelegate OnStrafingEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void DoStrafing();

	UFUNCTION()
	void EndStrafing();

	void ChangeStrafingDirection();

	FVector WorldDirectionForStrafing();

	virtual void CheckIsKnockUp() override;

	UFUNCTION(BlueprintCallable)
	void KnockUpEnd();

private:

	UPROPERTY(VisibleAnywhere)
	EStrafingDirection StrafingDir = EStrafingDirection::ESD_Front;

	UPROPERTY(VisibleAnywhere)
	bool bStrafing = false;

	float StrafingValue = 0.f;

	FTimerHandle StrafingTimer;

	float StrafingTime = 2.f;

	float JogAnimRate = 1.f;

public:

	float GetStrafingValue() const;

	float GetJogAnimRate() const;
};
