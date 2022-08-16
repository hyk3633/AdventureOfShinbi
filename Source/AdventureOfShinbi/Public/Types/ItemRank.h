#pragma once

UENUM(BlueprintType)
enum class EItemRank : uint8
{   // 초록 파랑 보라 분홍 주황
	EIR_Common UMETA(DisplayName = "Common"), 
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Unique UMETA(DisplayName = "Unique"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};