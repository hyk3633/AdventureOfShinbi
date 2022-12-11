

#include "System/NotifyFootStepSound.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UNotifyFootStepSound::UNotifyFootStepSound()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Emerald;
#endif 
}

void UNotifyFootStepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (FootStepSound.Num() == 0)
		return;

	const FTransform SocketTransform = MeshComp->GetSocketTransform(SocketName);
	
	FVector TraceStart = SocketTransform.GetLocation();
	FVector TraceEnd = SocketTransform.GetLocation() - FVector::UpVector * 30.f;

	TArray<AActor*> Ignores;
	Ignores.Add(MeshComp->GetOwner());

	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle
	(
		MeshComp->GetWorld(),
		TraceStart,
		TraceEnd,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		true,
		Ignores,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (HitResult.bBlockingHit)
	{
		if (HitResult.GetActor() == nullptr)
			return;

		const int8 Index = static_cast<int8>(HitResult.PhysMaterial->SurfaceType) - 1;
		if (Index > -1 && FootStepSound.Num() > Index)
		{
			if (FootStepSound[Index] == nullptr)
				return;

			UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), FootStepSound[Index], TraceStart);
		}
	}
}