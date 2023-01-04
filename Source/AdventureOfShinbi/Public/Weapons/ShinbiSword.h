
#pragma once

#include "CoreMinimal.h"
#include "Weapons/MeleeWeapon.h"
#include "ShinbiSword.generated.h"

class AProjectile;
class AProjectileShinbiWolf;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class UAudioComponent;
class AAOSCharacter;

/** 3가지의 스킬을 구분하기 위한 enum */
UENUM()
enum class EShinbiSkill : uint8
{
	ESS_WolfAttack,
	ESS_CirclingWolves,
	ESS_UltimateWolfAttack,

	ESS_MAX
};

UCLASS()
class ADVENTUREOFSHINBI_API AShinbiSword : public AMeleeWeapon
{
	GENERATED_BODY()
	
public:

	AShinbiSword();

	/** 
	* 스킬에 따라 얼마의 마나를 소모하는지 반환
	* @param ShinbiSkill 3가지의 스킬을 구분하는 enum
	*/
	float GetSkillMana(EShinbiSkill ShinbiSkill);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** 무기 상태 변경 */
	virtual void SetWeaponState(const EWeaponState State) override;

	/** 무기 발광 파티클 활성화 */
	void ActivateGlow();

	/** 스킬 : 써클링 울브즈의 투사체 Location, Rotation 업데이트 */
	void CirclingWolvesOn(float DeltaTime);

	/** 스킬 : 전방으로 늑대가 돌진 */
	UFUNCTION(BlueprintCallable)
	void WolfAttack();

	/** 스킬 : 캐릭터를 중심으로 5마리의 늑대가 회전하며 부딪히는 적에게 데미지를 입힘 */
	UFUNCTION(BlueprintCallable)
	void CirclingWolves();

	void PlayCirclingWolvesEffect();

	void CirclingWolvesDurationEnd();

	/** 스킬 : 캐릭터 전방에 있는 다수의 적에게 각각 3번씩 랜덤한 위치에서 늑대가 돌진하여 공격 */
	UFUNCTION(BlueprintCallable)
	void UltimateWolfRush();

	void PlayExplosionEffect();

	/** Wolf 투사체 소환 */
	void SpawnWolf();

	/** 투사체가 소환될 랜덤 Location, Rotation 계산 */
	void GetRandomSpawnPosition(FVector& SpawnLoc, FRotator& SpawnRot);

private:

	AAOSCharacter* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Effect")
	UParticleSystem* SwordGlowStart;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Effect")
	UParticleSystem* SwordGlowLoop;

	UParticleSystemComponent* SwordGlowLoopComp;

	/** SwordGlowLoop 파티클 재생에 딜레이를 주는 타이머 */
	FTimerHandle GlowTimer;

	/** 스킬 : 울프 어택 */

	/** Wolf 투사체 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	TSubclassOf<AProjectile> WolfProjClass;

	AProjectileShinbiWolf* WolfProj;

	/** 스킬 마나 소모량 */
	float WolfAttackMana = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	USoundCue* WolfAttackSound;

	/** 스킬 : 써클링 울브즈 */

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	UParticleSystem* CirclingParticle;

	UParticleSystemComponent* CirclingParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	UParticleSystem* LensFlareParticle;

	UParticleSystemComponent* LensFlareParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	USoundCue* CirclingWolvesSound;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Circling Wolves")
	USoundCue* CirclingLoopingSound;

	UAudioComponent* CirclingLoopingSoundComp;

	/** Wolf 투사체 배열 */
	TArray<AProjectileShinbiWolf*> CirclingWolvesArr;

	/** 캐릭터를 중심으로 각 투사체의 Location을 회전한 각도의 누적값 배열 */
	TArray<float> AngleAxis;

	/** 캐릭터를 중심으로 각 투사체의 Location을 회전할 각도 값 배열 */
	TArray<float> AngleOffset;

	/** 스킬 활성화 bool */
	bool bCirclingWolvesOn = false;

	/** Wolf 투사체의 위치 값 업데이트 보정치 */
	float Multiplier = -150.f;

	/** Wolf 투사체의 Rotation 회전 속도 */
	float RotateSpeed = -150.f;

	/** 스킬 지속 타이머 및 지속 시간 */
	FTimerHandle CirclingWolvesDurationTimer;
	float CirclingWolvesDurationTime = 10.f;

	/** 스킬 마나 소모량 */
	float CirclingWolvesMana = 100.f;

	/** 스킬 : 얼티메이트 울프 */

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* PortalParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* ExplosionParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* LightParticle;

	UParticleSystemComponent* LightParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	UParticleSystem* LightEndParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Ultimate Wolf Rush")
	USoundCue* PortalSound;

	/** 현재 공격 대상의 배열 인덱스 */
	int8 CurrentTargetIdx = 0;

	/** 현재 공격 사이클 */
	int8 CurrentAttackCycle = 0;

	/** 공격 사이클 최대 값 */
	int8 MaxAttackCycle = 3;

	/** 폭죽 파티클 지연 타이머 */
	FTimerHandle ExplosionTimer;

	/** 공격 지연 타이머 */
	FTimerHandle UltimateWolfAttackTimer;
	float UltimateWolfAttackTime = 0.3f;

	/** 탐지된 적 배열 */
	TArray<AActor*> Enemies;

	/** 스킬 마나 소모량 */
	float UltimateWolfAttackMana = 200.f;
};
