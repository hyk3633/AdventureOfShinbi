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

static FString SetItemRankToWidget(EItemRank Itemrank)
{
	switch (Itemrank)
	{
	case EItemRank::EIR_Common:
		return TEXT("일반");
	case EItemRank::EIR_Rare:
		return TEXT("희귀");
	case EItemRank::EIR_Unique:
		return TEXT("특별");
	case EItemRank::EIR_Legendary:
		return TEXT("전설");
	default:
		return TEXT("없음");
	}
}