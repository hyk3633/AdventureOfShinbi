// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/MeleeWeapon.h"
#include "Glaive.generated.h"

class AAOSCharacter;
class AProjectile;
class UParticleSystem;
class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API AGlaive : public AMeleeWeapon
{
	GENERATED_BODY()
	
public:

	AGlaive();

	virtual void SetWeaponState(const EWeaponState State) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 무기의 캡슐 트레이스 결과에 따라 데미지 적용 및 효과 재생 */
	virtual bool WeaponCapsuleTrace() override;

	/** 무기 스켈레탈 메쉬의 본 위치 값을 보간 */
	void InterpWeaponPartsRotator(float DeltaTime, bool SickleMode);

	/** 
	* 무기의 모드에 따른 값 설정
	* @param bRightButtonClicked 마우스 우클릭이 눌리면 true
	*/
	UFUNCTION()
	void FormChange(bool bRightButtonClicked);

	/** 마나가 소모량 이상 남아있는지 확인 */
	UFUNCTION(BlueprintCallable)
	void CheckMana();

	/** 투사체 발사 
	* @param 애니메이션 상에서 소켓 위치가 바닥에 있으면 true 
	*/
	UFUNCTION(BlueprintCallable)
	void EmitMagicalBall(bool bIsOpposite);

	void PlayEffect(UParticleSystem* Particle, USoundCue* Sound);

	void SpawnProjectile(FName SocketName = "MuzzleSocket");

	void ActivateSickleModeParticle();
	void DeactivateSickleModeParticle();

	void ActivateUltimateParticle();

	/** 스킬 : 전방의 다수의 적을 공중으로 띄우며 데미지를 입힘 CapsuleTrace 사용 */
	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack1();

	/** 스킬 : 전방의 다수의 적을 공중으로 띄우며 데미지를 입힘 SphereTrace 사용 */
	UFUNCTION(BlueprintCallable)
	void UltimateGlaiveAttack2();

	void PlayCameraShake();

	/** Trace 결과에 따라 데미지 적용, 이펙트 재생, LaunchCharacter 등을 수행 */
	void HandleTraceHitResult(TArray<FHitResult>& Results);

private:

	AAOSCharacter* WeaponOwner;

	/** 모드 토글 변수 */
	bool bSickleMode = false;

	/** 델리게이트가 바인딩 여부 */
	bool bDelBinded = false;

	/** 스킬 사용 가능여부 */
	bool bAbleMagicSkill = false;

	/** 무기 메쉬의 본 회전 속도 */
	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	float FormChangeSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	float SkillDamage = 1250.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Setting")
	TSubclassOf<UCameraShakeBase> CameraShakeUltimate;

	/** 현재의 무기 메쉬의 칼날 본 Rotation */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBladeValue = FRotator::ZeroRotator;

	/** 현재의 무기 메쉬의 몸체 본 Rotation */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator CurrentBodyValue = FRotator::ZeroRotator;

	/** 낫 모드의 Rotation 값 */
	FRotator SickleBodyRotator = FRotator(0.f, 0.f, 180.f);
	FRotator SickleBladeRotator = FRotator(90.f, 0.f, 0.f);

	/** 무기 메쉬의 본 Rotation 기본 값 */
	FRotator GlaiveRotator = FRotator::ZeroRotator;

	/** 투사체 템플릿 클래스 */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* SickleModeParticle;

	UParticleSystemComponent* SickleModeParticleComp;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* FormChangeParticle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	UParticleSystem* MuzzlFlashParticle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	USoundCue* SickleModeSound;

	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	USoundCue* EmitSound;

	/** 투사체 발사 마나 소모량 */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float EmitMagicalBallManaConsumption = 20.f;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateActivateParticle;

	UParticleSystemComponent* UltimateActivateParticleComp;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack1Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	UParticleSystem* UltimateGlaiveAttack2Particle;

	UPROPERTY(EditAnywhere, Category = "Glaive | Glaive Mode")
	USoundCue* GlaiveModeSound;

	/** 스킬 마나 소모량 */
	UPROPERTY(EditAnywhere, Category = "Glaive | Sickle Mode")
	float UltimateSkillManaConsumption = 100.f;

	/** 타격 횟수 */
	int8 HitStack = 0;

	/** 최대 타격 횟수 */
	int8 MaxHitStack = 5;

public:

	bool GetSickleMode() const;
	bool GetHitStackIsFull() const;
	float GetUltiSkillMana() const;

};
