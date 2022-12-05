

#include "EnemyMuriel/EnemyMuriel.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyAnimInstance.h"
#include "Player/AOSCharacter.h"
#include "System/AOSGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

AEnemyMuriel::AEnemyMuriel()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackingRotateRate = 100.f;
    NormalRotateRate = 100.f;

    SightStimulusExpireTime = 0.f;

    Damage = 250.f;
    Health = 3000.f;
    MaxHealth = 3000.f;
    Defense = 50.f;
    DefaultValue = 3.f;
    RandRangeValue = 20;
}

void AEnemyMuriel::BeginPlay()
{
	Super::BeginPlay();

    if (bWhite)
    {
        BuffDurationTime = 12.f;
        FriendlyTag = FName("White");
    }
    else
    {
        FriendlyTag = FName("Black");
    }
}

void AEnemyMuriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyMuriel::HandleStiffAndStun(bool IsHeadShot)
{
    if (bIsAttacking) 
        return;

    Super::HandleStiffAndStun(IsHeadShot);
}

void AEnemyMuriel::ResetAIState()
{
    Super::ResetAIState();

    GetWorldTimerManager().ClearTimer(BuffCoolTimer);
    GetWorldTimerManager().ClearTimer(SummonCoolTimer);
    GetWorldTimerManager().ClearTimer(TeleportCoolTimer);
    GetWorldTimerManager().ClearTimer(SkillShotCoolTimer);
    bBuffCoolTimeEnd = true;
    bSummonCoolTimeEnd = true;
    bSkillShotCoolTimeEnd = true;
    bTeleportMinionCoolTimeEnd = true;
}

void AEnemyMuriel::RangedAttack()
{
    if (bSkillShotCoolTimeEnd && FMath::RandRange(0.f, 1.f) <= 0.3)
    {
        FireSkillShot();
    }
    else
    {
        Super::RangedAttack();
    }
}

void AEnemyMuriel::SummonMinion()
{
    if (EnemyClassArr.Num() == 0)
        return;

    bSummonCoolTimeEnd = false;
    bIsAttacking = true;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    PlaySummonMontage();
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
        bIsAttacking = false;
        if (AIController)
        {
            AIController->UpdateAiInfo();
        }
        OnAttackEnd.Broadcast();
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
            EDrawDebugTrace::None,
            BoxHit,
            true
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
    if (EnemyAnim == nullptr || SummonMontage == nullptr) 
        return;

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
    bIsAttacking = false;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    OnAttackEnd.Broadcast();
}

void AEnemyMuriel::SummonCoolTimeEnd()
{
    bSummonCoolTimeEnd = true;
}

void AEnemyMuriel::ProvideBuff()
{
    bBuffCoolTimeEnd = false;
    bIsAttacking = true;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    PlayBuffMontage();
}

void AEnemyMuriel::Buff()
{
    UGameplayStatics::GetAllActorsOfClassWithTag(this, AEnemyCharacter::StaticClass(), FriendlyTag, TargetsToBuff);
    if (TargetsToBuff.Num() == 0)
    {
        bBuffCoolTimeEnd = true;
        bIsAttacking = false;
        if (AIController)
        {
            AIController->UpdateAiInfo();
        }
        OnAttackEnd.Broadcast();
        return;
    }

    for (AActor* const& TaggedActor : TargetsToBuff)
    {
        AEnemyCharacter* Friendly = Cast<AEnemyCharacter>(TaggedActor);
        if (Friendly)
        {
            if (GetDistanceTo(Friendly) <= 1000.f)
            {
                PlayBuffParticle(Friendly);
                if (bWhite)
                {
                    Friendly->ActivateHealing(Friendly->GetMaxHealth() * 0.5);
                }
                else
                {
                    Friendly->SetDamage(1.25f);
                }
            }
        }
    }

    GetWorldTimerManager().SetTimer(BuffDurationTimer, this, &AEnemyMuriel::BuffDurationEnd, BuffDurationTime);
    GetWorldTimerManager().SetTimer(BuffCoolTimer, this, &AEnemyMuriel::BuffCoolTimeEnd, BuffCoolTime);
}

void AEnemyMuriel::PlayBuffMontage()
{
    if (EnemyAnim == nullptr || BuffMontage == nullptr) 
        return;

    EnemyAnim->Montage_Play(BuffMontage);
}

void AEnemyMuriel::OnBuffMontageEnded()
{
    bIsAttacking = false;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    OnAttackEnd.Broadcast();
}

void AEnemyMuriel::BuffCoolTimeEnd()
{
	bBuffCoolTimeEnd = true;
}

void AEnemyMuriel::BuffDurationEnd()
{
    if (BuffParticleComponents.Num() > 0)
    {
        for (int8 i = 0; i < BuffParticleComponents.Num(); i++)
        {
            BuffParticleComponents[i]->Deactivate();
        }
        BuffParticleComponents.Empty();
    }
    
    if (bWhite == false)
    {
        for (AActor* Target : TargetsToBuff)
        {
            AEnemyCharacter* BuffedTarget = Cast<AEnemyCharacter>(Target);
            if (Target)
            {
                BuffedTarget->SetDamage();
            }
        }
    }

    TargetsToBuff.Empty();
}

void AEnemyMuriel::PlayBuffParticle(const ACharacter* BuffTarget)
{
    if (BuffStartParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BuffStartParticle, BuffTarget->GetActorLocation(), BuffTarget->GetActorRotation(), true);
    }
    if (BuffParticle)
    {
        BuffParticleComponents.Add(UGameplayStatics::SpawnEmitterAttached
        (
            BuffParticle,
            BuffTarget->GetMesh(),
            FName(),
            FVector::UpVector * 50.f,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset
        ));
    }
}

void AEnemyMuriel::FireSkillShot()
{
    bSkillShotCoolTimeEnd = false;
    bIsAttacking = true;
    Damage = 400.f;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    PlaySkillShotFireMontage();
    GetWorldTimerManager().SetTimer(SkillShotCoolTimer, this, &AEnemyMuriel::SkillShotCoolTimeEnd, SkillShotCoolTime);
}

void AEnemyMuriel::PlaySkillShotFireMontage()
{
    if (EnemyAnim == nullptr || SkillShotFireMontage == nullptr) 
        return;

    EnemyAnim->Montage_Play(SkillShotFireMontage);
}

void AEnemyMuriel::FireSkillShotProjectile()
{
    ProjectileFire(SkillProjectileClass);
}

void AEnemyMuriel::OnSkillShotFireMontageEnded()
{
    bIsAttacking = false;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    Damage = 250.f;
    OnAttackEnd.Broadcast();
}

void AEnemyMuriel::SkillShotCoolTimeEnd()
{
    bSkillShotCoolTimeEnd = true;
}

void AEnemyMuriel::FindTeleportPosition()
{
    AAOSCharacter* AC = Cast<AAOSCharacter>(AiInfo.TargetPlayer);
    if (AC)
    {
        bool bTeleportIsSafe = CheckSpawnPosition(TeleportPosition, AC, 200.f);
        if (bTeleportIsSafe == false)
        {
            bIsAttacking = false;
            if (AIController)
            {
                AIController->UpdateAiInfo();
            }
            return;
        }

        TArray<AActor*> Allies;
        UGameplayStatics::GetAllActorsWithTag(this, FriendlyTag, Allies);
        if (Allies.Num() == 0)
        {
            bIsAttacking = false;
            if (AIController)
            {
                AIController->UpdateAiInfo();
            }
            return;
        }

        float FarestDistance = 0.f;
        for (AActor* const& TaggedActor : Allies)
        {
            if (FarestDistance < TaggedActor->GetDistanceTo(AC))
            {
                FarestDistance = TaggedActor->GetDistanceTo(AC);
                FarestMinion = TaggedActor;
            }
        }

        TeleportRotation = (AC->GetActorForwardVector() * -1.f).Rotation();

        bTeleportMinionCoolTimeEnd = false;
        bIsAttacking = true;
        if (AIController)
        {
            AIController->UpdateAiInfo();
        }

        PlayTeleportMinionMontage();
        TeleportMinionToPlayer();

        GetWorldTimerManager().SetTimer(TeleportCoolTimer, this, &AEnemyMuriel::TeleportMinionCoolTimeEnd, TeleportCoolTime);
    }
}

void AEnemyMuriel::TeleportMinionToPlayer()
{
    PlayTeleportMinionEffect();
    AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
    EC->GetMesh()->SetVisibility(false);

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
    if (EnemyAnim == nullptr || TeleportMinionMontage == nullptr) 
        return;

    EnemyAnim->Montage_Play(TeleportMinionMontage);
}

void AEnemyMuriel::OnTeleportMinionMontageEnded()
{
    bIsAttacking = false;
    if (AIController)
    {
        AIController->UpdateAiInfo();
    }
    OnAttackEnd.Broadcast();
}

void AEnemyMuriel::TeleportMinionDelayEnd()
{
    AEnemyCharacter* EC = Cast<AEnemyCharacter>(FarestMinion);
    if (EC == nullptr) 
        return;
    
    FarestMinion->SetActorLocation(TeleportPosition);
    FarestMinion->SetActorRotation(TeleportRotation);
    EC->GetMesh()->SetVisibility(true);
    PlayTeleportMinionEffect();
}

void AEnemyMuriel::TeleportMinionCoolTimeEnd()
{
    bTeleportMinionCoolTimeEnd = true;
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

FName AEnemyMuriel::GetFriendlyTag() const
{
    return FriendlyTag;
}

bool AEnemyMuriel::GetTeleportMinionCoolTimeEnd() const
{
    return bTeleportMinionCoolTimeEnd;
}