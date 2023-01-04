
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

/** 3������ ��ų�� �����ϱ� ���� enum */
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
	* ��ų�� ���� ���� ������ �Ҹ��ϴ��� ��ȯ
	* @param ShinbiSkill 3������ ��ų�� �����ϴ� enum
	*/
	float GetSkillMana(EShinbiSkill ShinbiSkill);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** ���� ���� ���� */
	virtual void SetWeaponState(const EWeaponState State) override;

	/** ���� �߱� ��ƼŬ Ȱ��ȭ */
	void ActivateGlow();

	/** ��ų : ��Ŭ�� ������� ����ü Location, Rotation ������Ʈ */
	void CirclingWolvesOn(float DeltaTime);

	/** ��ų : �������� ���밡 ���� */
	UFUNCTION(BlueprintCallable)
	void WolfAttack();

	/** ��ų : ĳ���͸� �߽����� 5������ ���밡 ȸ���ϸ� �ε����� ������ �������� ���� */
	UFUNCTION(BlueprintCallable)
	void CirclingWolves();

	void PlayCirclingWolvesEffect();

	void CirclingWolvesDurationEnd();

	/** ��ų : ĳ���� ���濡 �ִ� �ټ��� ������ ���� 3���� ������ ��ġ���� ���밡 �����Ͽ� ���� */
	UFUNCTION(BlueprintCallable)
	void UltimateWolfRush();

	void PlayExplosionEffect();

	/** Wolf ����ü ��ȯ */
	void SpawnWolf();

	/** ����ü�� ��ȯ�� ���� Location, Rotation ��� */
	void GetRandomSpawnPosition(FVector& SpawnLoc, FRotator& SpawnRot);

private:

	AAOSCharacter* WeaponOwner;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Effect")
	UParticleSystem* SwordGlowStart;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Effect")
	UParticleSystem* SwordGlowLoop;

	UParticleSystemComponent* SwordGlowLoopComp;

	/** SwordGlowLoop ��ƼŬ ����� �����̸� �ִ� Ÿ�̸� */
	FTimerHandle GlowTimer;

	/** ��ų : ���� ���� */

	/** Wolf ����ü Ŭ���� */
	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	TSubclassOf<AProjectile> WolfProjClass;

	AProjectileShinbiWolf* WolfProj;

	/** ��ų ���� �Ҹ� */
	float WolfAttackMana = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shinbi Sword | Wolf Attack")
	USoundCue* WolfAttackSound;

	/** ��ų : ��Ŭ�� ����� */

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

	/** Wolf ����ü �迭 */
	TArray<AProjectileShinbiWolf*> CirclingWolvesArr;

	/** ĳ���͸� �߽����� �� ����ü�� Location�� ȸ���� ������ ������ �迭 */
	TArray<float> AngleAxis;

	/** ĳ���͸� �߽����� �� ����ü�� Location�� ȸ���� ���� �� �迭 */
	TArray<float> AngleOffset;

	/** ��ų Ȱ��ȭ bool */
	bool bCirclingWolvesOn = false;

	/** Wolf ����ü�� ��ġ �� ������Ʈ ����ġ */
	float Multiplier = -150.f;

	/** Wolf ����ü�� Rotation ȸ�� �ӵ� */
	float RotateSpeed = -150.f;

	/** ��ų ���� Ÿ�̸� �� ���� �ð� */
	FTimerHandle CirclingWolvesDurationTimer;
	float CirclingWolvesDurationTime = 10.f;

	/** ��ų ���� �Ҹ� */
	float CirclingWolvesMana = 100.f;

	/** ��ų : ��Ƽ����Ʈ ���� */

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

	/** ���� ���� ����� �迭 �ε��� */
	int8 CurrentTargetIdx = 0;

	/** ���� ���� ����Ŭ */
	int8 CurrentAttackCycle = 0;

	/** ���� ����Ŭ �ִ� �� */
	int8 MaxAttackCycle = 3;

	/** ���� ��ƼŬ ���� Ÿ�̸� */
	FTimerHandle ExplosionTimer;

	/** ���� ���� Ÿ�̸� */
	FTimerHandle UltimateWolfAttackTimer;
	float UltimateWolfAttackTime = 0.3f;

	/** Ž���� �� �迭 */
	TArray<AActor*> Enemies;

	/** ��ų ���� �Ҹ� */
	float UltimateWolfAttackMana = 200.f;
};
