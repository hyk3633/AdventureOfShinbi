

#include "Weapons/Glaive.h"
#include "Weapons/Projectile.h"
#include "Player/AOSCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "EnemyMuriel/EnemyMuriel.h"
#include "EnemyBoss/EnemyBoss.h"
#include "Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "AdventureOfShinbi/AdventureOfShinbi.h"

AGlaive::AGlaive()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponType = EWeaponType::EWT_Glaive;
	CapsuleRadius = 50.f;
	CapsuleHalfHeight = 50.f;

	Damage = 400.f;
}

void AGlaive::BeginPlay()
{
	Super::BeginPlay();
}

void AGlaive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InterpWeaponPartsRotator(DeltaTime, bSickleMode);
}

bool AGlaive::WeaponCapsuleTrace()
{
	const bool bHit = Super::WeaponCapsuleTrace();

	if (bHit)
	{
		// 낫 모드가 아니면 타격 횟수 카운트
		if (bSickleMode == false)
		{
			HitStack++;
		}

		// 최대 타격 횟수에 도달하면 파티클 활성화
		if (HitStack == MaxHitStack)
		{
			ActivateUltimateParticle();
		}
	}

	return bHit;
}

// 모드에 따라 메쉬의 본 Rotation 값 보간
void AGlaive::InterpWeaponPartsRotator(float DeltaTime, bool SickleMode)
{
	if (SickleMode)
	{
		CurrentBladeValue = FMath::RInterpTo(CurrentBladeValue, SickleBladeRotator, DeltaTime, FormChangeSpeed);
		CurrentBodyValue = FMath::RInterpTo(CurrentBodyValue, SickleBodyRotator, DeltaTime, FormChangeSpeed);
	}
	else
	{
		CurrentBladeValue = FMath::RInterpTo(CurrentBladeValue, GlaiveRotator, DeltaTime, FormChangeSpeed);
		CurrentBodyValue = FMath::RInterpTo(CurrentBodyValue, GlaiveRotator, DeltaTime, FormChangeSpeed);
	}
}

void AGlaive::FormChange(bool bRightButtonClicked)
{
	if (bRightButtonClicked == false || WeaponOwner->GetIsAnimationPlaying())
		return;

	// 낫 모드 활성화
	if (bSickleMode == false)
	{
		bSickleMode = true;
		PlayEffect(FormChangeParticle, SickleModeSound);
		ActivateSickleModeParticle();
		if (HitStack == MaxHitStack)
		{
			UltimateActivateParticleComp->Deactivate();
		}
	}
	else // 낫 모드 비활성화
	{
		bSickleMode = false;
		HitStack = 0;
		PlayEffect(FormChangeParticle, GlaiveModeSound);
		DeactivateSickleModeParticle();
		if (HitStack == MaxHitStack)
		{
			ActivateUltimateParticle();
		}
	}

}

void AGlaive::CheckMana()
{
	if (bSickleMode == false || WeaponOwner == nullptr)
	{
		bAbleMagicSkill = false;
		return;
	}

	bAbleMagicSkill = WeaponOwner->GetCombatComp()->SpendMana(EmitMagicalBallManaConsumption);
}

void AGlaive::EmitMagicalBall(bool bIsOpposite)
{
	if (bSickleMode == false || bAbleMagicSkill == false)
		return;

	PlayEffect(MuzzlFlashParticle, EmitSound);

	if (bIsOpposite)
	{
		SpawnProjectile("MuzzleSocket180");
	}
	else
	{
		SpawnProjectile();
	}
}

void AGlaive::PlayEffect(UParticleSystem* Particle, USoundCue* Sound)
{
	const USkeletalMeshSocket* MuzzleSocket = ItemMesh->GetSocketByName(FName("MuzzleSocket"));
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(ItemMesh);

	if (Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, SocketTransform.GetLocation());
	}
}

void AGlaive::SpawnProjectile(FName SocketName)
{
	PlayEffect(MuzzlFlashParticle, EmitSound);

	const USkeletalMeshSocket* MuzzleSocket = ItemMesh->GetSocketByName(SocketName);
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(ItemMesh);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (InstigatorPawn == nullptr)
		return;

	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), SpawnParams);
		}
	}

}

void AGlaive::ActivateSickleModeParticle()
{
	const USkeletalMeshSocket* MuzzleSocket = ItemMesh->GetSocketByName(FName("MuzzleSocket"));
	if (MuzzleSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(ItemMesh);

	if (SickleModeParticle)
	{
		SickleModeParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			SickleModeParticle, 
			ItemMesh, 
			NAME_None, 
			SocketTransform.GetLocation(),
			FRotator::ZeroRotator, 
			EAttachLocation::KeepWorldPosition, 
			false
		);
	}
}

void AGlaive::DeactivateSickleModeParticle()
{
	SickleModeParticleComp->Deactivate();
}

void AGlaive::ActivateUltimateParticle()
{
	if (UltimateActivateParticle == nullptr)
		return;

	if(UltimateActivateParticleComp == nullptr)
	{
		const USkeletalMeshSocket* ChargeUpSocket = ItemMesh->GetSocketByName(FName("ChargeUpSocket"));
		if (ChargeUpSocket == nullptr) return;
		const FTransform SocketTransform = ChargeUpSocket->GetSocketTransform(ItemMesh);

		UltimateActivateParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			UltimateActivateParticle,
			ItemMesh,
			NAME_None,
			SocketTransform.GetLocation(),
			SocketTransform.GetRotation().Rotator(),
			EAttachLocation::KeepWorldPosition,
			false,
			EPSCPoolMethod::None,
			true
		);
	}
	else
	{
		UltimateActivateParticleComp->Activate();
	}
}

void AGlaive::UltimateGlaiveAttack1()
{
	if (WeaponOwner == nullptr)
		return;

	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_PlayerWeaponTrace);

	FVector TraceStart = WeaponOwner->GetActorLocation() + WeaponOwner->GetActorForwardVector() * 300.f;
	TraceStart.Z = 20.f;
	const FVector TraceEnd = TraceStart + WeaponOwner->GetActorForwardVector() * 600.f;

	TArray<FHitResult> HitArr;
	const bool bHit = UKismetSystemLibrary::CapsuleTraceMulti
	(
		this,
		TraceStart,
		TraceEnd,
		180.f,
		100.f,
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitArr,
		true
	);

	PlayCameraShake();

	if (UltimateGlaiveAttack2Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			this,
			UltimateGlaiveAttack2Particle,
			TraceStart,
			WeaponOwner->GetActorRotation(),
			true
		);
	}

	if (bHit)
	{
		HandleTraceHitResult(HitArr);
	}
}

void AGlaive::UltimateGlaiveAttack2()
{
	if (WeaponOwner == nullptr)
		return;

	UCollisionProfile* Profile = UCollisionProfile::Get();
	ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECC_PlayerWeaponTrace);

	FVector AttackPoint = WeaponOwner->GetActorLocation() + WeaponOwner->GetActorForwardVector() * 700.f;
	AttackPoint.Z = 20.f;

	TArray<FHitResult> HitArr;
	const bool bHit = UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		AttackPoint,
		AttackPoint,
		250.f,
		TraceType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitArr,
		true
	);

	PlayCameraShake();

	if (UltimateGlaiveAttack1Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			this,
			UltimateGlaiveAttack1Particle,
			AttackPoint + WeaponOwner->GetActorForwardVector() * 110.f,
			WeaponOwner->GetActorRotation() + FRotator(90.f,0.f,0.f),
			true
		);
	}

	if (bHit)
	{
		HandleTraceHitResult(HitArr);
	}
}

void AGlaive::PlayCameraShake()
{
	if (CameraShakeUltimate)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeUltimate);
	}
}

void AGlaive::HandleTraceHitResult(TArray<FHitResult>& Results)
{
	HitStack = 0;
	UltimateActivateParticleComp->Deactivate();

	// 트레이스가 적중한 적 액터에게만 데미지 적용, 이펙트 재생, LaunchCharacter 수행 (보스 액터는 제외)
	TArray<AActor*> Actors;
	for (FHitResult Hit : Results)
	{
		if (Actors.Contains(Hit.GetActor()) == false)
		{
			Actors.Add(Hit.GetActor());

			PlayAttackEffect(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			UGameplayStatics::ApplyPointDamage
			(
				Hit.GetActor(),
				SkillDamage,
				Hit.ImpactPoint,
				Hit,
				WeaponOwner->GetController(),
				WeaponOwner,
				UDamageType::StaticClass()
			);
			ACharacter* Cha = Cast<ACharacter>(Hit.GetActor());
			if (Cha)
			{
				AEnemyMuriel* EM = Cast<AEnemyMuriel>(Cha);
				if (EM)
					continue;
				AEnemyBoss* EB = Cast<AEnemyBoss>(Cha);
				if (EB)
					continue;
				Cha->LaunchCharacter(Cha->GetActorUpVector() * 700.f, true, false);
			}
		}
	}
}

bool AGlaive::GetSickleMode() const
{
	return bSickleMode;
}

bool AGlaive::GetHitStackIsFull() const
{
	if (HitStack >= MaxHitStack)
		return true;
	else
		return false;
}

float AGlaive::GetUltiSkillMana() const
{
	return UltimateSkillManaConsumption;
}

void AGlaive::SetWeaponState(const EWeaponState State)
{
	EWeaponState PrevState = WeaponState;

	Super::SetWeaponState(State);

	if (State == EWeaponState::EWS_Equipped)
	{
		if (WeaponOwner && WeaponOwner->DAimButtonPressed.IsBound() == false)
		{
			WeaponOwner->DAimButtonPressed.BindUObject(this, &AGlaive::FormChange);
		}
	}
	else if (PrevState == EWeaponState::EWS_Equipped && State == EWeaponState::EWS_Field)
	{
		if (WeaponOwner && WeaponOwner->DAimButtonPressed.IsBound())
		{
			WeaponOwner->DAimButtonPressed.Unbind();

			if (UltimateActivateParticleComp)
			{
				UltimateActivateParticleComp->DestroyComponent();
			}
		}
	}
}

