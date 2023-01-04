
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyStrafing.generated.h"

/**
 * ȸ�� �⵿�� ������ �� ĳ����
 */

DECLARE_MULTICAST_DELEGATE(FOnStrafingEndDelegate);

/** ȸ�� �⵿ ���� enum */
UENUM(BlueprintType)
enum class EStrafingDirection : uint8
{
	ESD_Front UMETA(DisplayName = "Front"),
	ESD_Left UMETA(DisplayName = "Left"),
	ESD_Right UMETA(DisplayName = "Right"),
	ESD_RDiagonal UMETA(DisplayName = "RDiagonal"),
	ESD_LDiagonal UMETA(DisplayName = "LDiagonal"),

	ESD_MAX UMETA(Hidden)
};

UCLASS()
class ADVENTUREOFSHINBI_API AEnemyStrafing : public AEnemyRanged
{
	GENERATED_BODY()
	
public:

	AEnemyStrafing();

	/** BT_Task���� ȣ���ϴ� ȸ�� �⵿ �Լ� */
	void StartStrafing();

	FOnStrafingEndDelegate OnStrafingEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ResetAIState() override;

	/** ȸ�� �⵿ ���� */
	virtual void DoStrafing();

	/** ĳ���͸� �����̰� ���� �����̽����� ����ϴ� Axis���� ��� */
	void SetStrafingValue();

	/** ȸ�� �⵿�� �������� �˻� */
	virtual bool CheckStrafingCondition();

	UFUNCTION()
	void EndStrafing();

	/** ȸ�� �⵿ ���� ���� */
	virtual void ChangeStrafingDirection();

	/** ȸ�� �⵿���� ������ ���� ���͸� ��ȯ */
	FVector WorldDirectionForStrafing();

	UFUNCTION(BlueprintCallable)
	void KnockUpEnd();

protected:

	/** ȸ�� �⵿�� �����ϴ� enum */
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Strafing")
	EStrafingDirection StrafingDir = EStrafingDirection::ESD_Front;

	/** ȸ�� �⵿ ���� ���� */
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Strafing")
	bool bStrafing = false;

	/** ���� �����̽��� Axis �� */
	float StrafingValue = 0.f;

	/** �߰� �ӵ� */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float JogAnimRate = 1.f;

	/** ȸ�� �⵿ �ӵ� */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float StrafingAnimRate = 0.8f;

	/** �߰� �� �ִϸ��̼� �ӵ� */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float ChaseAnimRate = 1.1f;

	FTimerHandle StrafingTimer;

	/** ȸ�� �⵿ ���� ��ȯ ������ ���� �ð� */
	float StrafingTime = 2.f;

public:

	float GetStrafingValue() const;
	float GetJogAnimRate() const;
	bool GetStrafing() const;
};
