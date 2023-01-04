
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyRanged.h"
#include "EnemyStrafing.generated.h"

/**
 * 회피 기동이 가능한 적 캐릭터
 */

DECLARE_MULTICAST_DELEGATE(FOnStrafingEndDelegate);

/** 회피 기동 방향 enum */
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

	/** BT_Task에서 호출하는 회피 기동 함수 */
	void StartStrafing();

	FOnStrafingEndDelegate OnStrafingEnd;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ResetAIState() override;

	/** 회피 기동 시작 */
	virtual void DoStrafing();

	/** 캐릭터를 움직이고 블렌드 스페이스에서 사용하는 Axis값을 계산 */
	void SetStrafingValue();

	/** 회피 기동이 가능한지 검사 */
	virtual bool CheckStrafingCondition();

	UFUNCTION()
	void EndStrafing();

	/** 회피 기동 방향 변경 */
	virtual void ChangeStrafingDirection();

	/** 회피 기동에서 움직일 방향 벡터를 반환 */
	FVector WorldDirectionForStrafing();

	UFUNCTION(BlueprintCallable)
	void KnockUpEnd();

protected:

	/** 회피 기동을 설정하는 enum */
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Strafing")
	EStrafingDirection StrafingDir = EStrafingDirection::ESD_Front;

	/** 회피 기동 수행 여부 */
	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Strafing")
	bool bStrafing = false;

	/** 블렌드 스페이스의 Axis 값 */
	float StrafingValue = 0.f;

	/** 추격 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float JogAnimRate = 1.f;

	/** 회피 기동 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float StrafingAnimRate = 0.8f;

	/** 추격 시 애니메이션 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Strafing")
	float ChaseAnimRate = 1.1f;

	FTimerHandle StrafingTimer;

	/** 회피 기동 방향 전환 까지의 유지 시간 */
	float StrafingTime = 2.f;

public:

	float GetStrafingValue() const;
	float GetJogAnimRate() const;
	bool GetStrafing() const;
};
