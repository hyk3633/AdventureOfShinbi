// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOFSHINBI_API UEnemyAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
public:

	virtual void HandleExpiredStimulus(FAIStimulus& StimulusStore) override;
};
