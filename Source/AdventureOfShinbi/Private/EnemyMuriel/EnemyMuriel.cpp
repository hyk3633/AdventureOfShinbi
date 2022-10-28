

#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

AEnemyMuriel::AEnemyMuriel()
{

}

void AEnemyMuriel::BeginPlay()
{
	Super::BeginPlay();

	FriendlyTag = bWhite ? FName("White") : FName("Black");
}

void AEnemyMuriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyMuriel::TakePointDamage(AActor* DamagedActor, float DamageReceived, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
    Super::TakePointDamage(DamagedActor, DamageReceived, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);

    const float RandPercentage = FMath::RandRange(0.f, 1.0f);
    if (bTeleportMinionCoolTimeEnd && RandPercentage <= 0.3f)
    {
        bAbleTeleportMinion = true;
    }
}

void AEnemyMuriel::HandleStiffAndStun(FName& BoneName)
{
    if (bIsCasting) return;

    Super::HandleStiffAndStun(BoneName);
}

void AEnemyMuriel::ProvideBuff()
{
    bBuffCoolTimeEnd = false;
    bIsCasting = true;
    PlayBuffMontage();
}

void AEnemyMuriel::SummonMinion() // 수정
{
    if (EnemyClassArr.Num() == 0) return;

    bSummonCoolTimeEnd = false;
    bIsCasting = true;
    PlaySummonMontage();
}

void AEnemyMuriel::FireSkillShot()
{
    bSkillShotCoolTimeEnd = false;
    bIsAttacking = true;

    PlaySkillShotFireMontage();
    GetWorldTimerManager().SetTimer(SkillShotCoolTimer, this, &AEnemyMuriel::SkillShotCoolTimeEnd, SkillShotCoolTime);
}

void AEnemyMuriel::FindTeleportPosition()
{    
    AAOSCharacter* AC = Cast<AAOSCharacter>(AiInfo.TargetPlayer);
    if (AC)
    {
        bool bTeleportIsSafe = CheckSpawnPosition(TeleportPosition, AC, 200.f);
        if (bTeleportIsSafe == false) return;

        TArray<AActor*> Allies;
        UGameplayStatics::GetAllActorsWithTag(this, FriendlyTag, Allies);
        if (Allies.Num() == 0) return;

        float FarestDistance = 0.f;
        for (AActor* const& TaggedActor : Allies)
        {
            if (FarestDistance < TaggedActor->GetDistanceTo(AC))
            {
                FarestDistance = TaggedActor->GetDistanceTo(AC);
                FarestMinion = TaggedActor;
            }
        }

        TeleportRotation = AC->GetVelocity().Rotation();
        TeleportRotation.Yaw += RotationDir[RotationIdx];

        bAbleTeleportMinion = false;
        bIsCasting = true;

        PlayTeleportMinionMontage();
        TeleportMinionToPlayer();

        GetWorldTimerManager().SetTimer(TeleportCoolTimer, this, &AEnemyMuriel::TeleportMinionCoolTimeEnd, TeleportCoolTime);
    }
    
}

void AEnemyMuriel::Summon()
{
    int8 SummonSuccess = 0;
    for (int8 i = 0; i < 2; i++)
    {
        FVector SummonPosition;
        const bool bSummonIsSafe = CheckSpawnPosition(SummonPosition, this, 150.f);
        if (bSummonIsSafe == false)
            continue;

        int8 RandIdx = FMath::RandRange(0.f, 1.f) <= 0.8f ? FMath::RandRange(0, 1) : FMath::RandRange(2, 3);

        if (EnemyClassArr[RandIdx])
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = GetOwner();
            SpawnParams.Instigator = this;

            UWorld* World = GetWorld();
            if (World)
            {
                PlaySummonParticle(SummonPosition);
                World->SpawnActor<AEnemyCharacter>(EnemyClassArr[RandIdx], SummonPosition, GetActorRotation(), SpawnParams);
                SummonSuccess++;
            }
        }
    }

    if (SummonSuccess > 0)
    {
        GetWorldTimerManager().SetTimer(SummonCoolTimer, this, &AEnemyMuriel::SummonCoolTimeEnd, SummonCoolTime);
    }
    else
    {
        bSummonCoolTimeEnd = true;
    }
}

bool AEnemyMuriel::CheckSpawnPosition(FVector& SafePosition, AActor* CenterActor, float PositionOffset)
{
    UCollisionProfile* Profile = UCollisionProfile::Get();
    ETraceTypeQuery TraceType = Profile->ConvertToTraceType(ECollisionChannel::ECC_Visibility);
    
    bool bSecondLoop = false;
    for (int8 i = 0; i < 8; i++)
    {
        FHitResult BoxHit;

        const FVector BoxTraceStart = CenterActor->GetActorLocation() +
                                      (CenterActor->GetActorForwardVector() * BoxTraceDir[i][0] * PositionOffset) +
                                      (CenterActor->GetActorRightVector() * BoxTraceDir[i][1] * PositionOffset);
        const FVector BoxTraceEnd = BoxTraceStart + FVector(0.f, 0.f, 150.f);

        UKismetSystemLibrary::BoxTraceSingle
        (
            this,
            BoxTraceStart,
            BoxTraceEnd,
            FVector(70.f, 70.f, 0.f),
            CenterActor->GetActorRotation(),
            TraceType,
            false,
            TArray<AActor*>(),
            EDrawDebugTrace::ForDuration,
            BoxHit,
            true,
            FLinearColor::Red,
            FLinearColor::Blue,
            3.f
        );

        if (BoxHit.bBlockingHit == false)
        {
            SafePosition = BoxTraceStart;
            RotationIdx = i;
            return true;
        }

        if (bSecondLoop == false)
        {
            PositionOffset *= 2;
            i = 0;
        }
    }

    return false;
}

void AEnemyMuriel::PlaySummonMontage()
{
    if (EnemyAnim == nullptr || SummonMontage == nullptr) return;

    EnemyAnim->Montage_Play(SummonMontage);
}

void AEnemyMuriel::PlaySummonParticle(FVector Position)
{
    if (SummonParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SummonParticle, Position);
    }
}

void AEnemyMuriel::OnSummonMontageEnded()
{
    OnSkillEnd.Broadcast();
    bIsCasting = false;
}

void AEnemyMuriel::SummonCoolTimeEnd()
{
    bSummonCoolTimeEnd = true;
}

void AEnemyMuriel::Buff()
{
    TArray<AActor*> Allies;
    UGameplayStatics::GetAllActorsWithTag(this, FriendlyTag, Allies);
    if (Allies.Num() == 0) return;

    for (AActor* const& TaggedActor : Allies)
    {
        AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
        if (Friendly)
        {
            if (GetDistanceTo(Friendly) <= 1000.f)
            {
                if (bWhite)
                {
                    Friendly->ActivateHealing(Friendly->GetMaxHealth() * 0.3);
                }
                else
                {
                    Friendly->ActivateDamageUp(1.2f);
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(BuffTimer, this, &AEnemyMuriel::BuffCoolTimeEnd, BuffCoolTime);
}

void AEnemyMuriel::PlayBuffMontage()
{
    if (EnemyAnim == nullptr || BuffMontage == nullptr) return;

    EnemyAnim->Montage_Play(BuffMontage);
}

void AEnemyMuriel::OnBuffMontageEnded()
{
    OnSkillEnd.Broadcast();
    bIsCasting = false;
}

void AEnemyMuriel::BuffCoolTimeEnd()
{
	bBuffCoolTimeEnd = true;
}

void AEnemyMuriel::PlaySkillShotFireMontage()
{
    if (EnemyAnim == nullptr || SkillShotFireMontage == nullptr) return;

    EnemyAnim->Montage_Play(SkillShotFireMontage);
}

void AEnemyMuriel::FireSkillShotProjectile()
{
    ProjectileFire(SkillProjectileClass);
}

void AEnemyMuriel::OnSkillShotFireMontageEnded()
{
    bIsAttacking = false;
    OnAttackEnd.Broadcast();
}

void AEnemyMuriel::SkillShotCoolTimeEnd()
{
    bSkillShotCoolTimeEnd = true;
}

void AEnemyMuriel::TeleportMinionToPlayer()
{
    PlayTeleportMinionEffect();
    AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
    EC->GetMesh()->SetVisibility(false);
    // 텔레포트 시 행동 잠금

    GetWorldTimerManager().SetTimer(TeleportTimer, this, &AEnemyMuriel::TeleportMinionDelayEnd, TeleportDelayTime);
}

void AEnemyMuriel::PlayTeleportMinionEffect()
{
    if (TeleportMinionParticle1)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportMinionParticle1, FarestMinion->GetActorLocation(), FarestMinion->GetActorRotation());
    }
    if (TeleportMinionParticle2)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportMinionParticle2, FarestMinion->GetActorLocation(), FarestMinion->GetActorRotation());
    }
    if (TeleportSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, FarestMinion->GetActorLocation());
    }
}

void AEnemyMuriel::PlayTeleportMinionMontage()
{
    if (EnemyAnim == nullptr || TeleportMinionMontage == nullptr) return;

    EnemyAnim->Montage_Play(TeleportMinionMontage);
}

void AEnemyMuriel::OnTeleportMinionMontageEnded()
{
    OnSkillEnd.Broadcast();

    bIsCasting = false;
}

void AEnemyMuriel::TeleportMinionDelayEnd()
{
    AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
    if (EC == nullptr) return;
    
    // 텔레포트 시 행동 해제
    FarestMinion->SetActorLocation(TeleportPosition);
    FarestMinion->SetActorRotation(TeleportRotation);
    EC->GetMesh()->SetVisibility(true);
    PlayTeleportMinionEffect();
}

void AEnemyMuriel::TeleportMinionCoolTimeEnd()
{
    bTeleportMinionCoolTimeEnd = true;
}

FName AEnemyMuriel::GetFriendlyTag() const
{
	return FriendlyTag;
}

bool AEnemyMuriel::GetMurielType() const
{
    return bWhite;
}

float AEnemyMuriel::GetSkillShotDurationTime() const
{
    return SkillShotDurationTime;
}

bool AEnemyMuriel::GetBuffCoolTimeEnd() const
{
	return bBuffCoolTimeEnd;
}

bool AEnemyMuriel::GetSummonCoolTimeEnd() const
{
    return bSummonCoolTimeEnd;
}

bool AEnemyMuriel::GetSkillShotCoolTimeEnd() const
{
    return bSkillShotCoolTimeEnd;
}
