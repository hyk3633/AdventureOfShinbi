
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NotifyFootStepSound.generated.h"

/**
 * 발자국 소리를 출력하기 위한 애님 노티파이 클래스
 */

class USoundCue;

UCLASS()
class ADVENTUREOFSHINBI_API UNotifyFootStepSound : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UNotifyFootStepSound();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Volume;

private:

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	TArray<USoundCue*> FootStepSound;

};
