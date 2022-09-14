// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIPerceptionComponent.h"

void UEnemyAIPerceptionComponent::HandleExpiredStimulus(FAIStimulus& StimulusStore)
{
	
	UE_LOG(LogTemp, Warning, TEXT("%s, %d"), *StimulusStore.Type.Name.ToString(), StimulusStore.IsExpired());
}