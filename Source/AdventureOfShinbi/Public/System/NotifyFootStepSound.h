
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NotifyFootStepSound.generated.h"

/**
 * ���ڱ� �Ҹ��� ����ϱ� ���� �ִ� ��Ƽ���� Ŭ����
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