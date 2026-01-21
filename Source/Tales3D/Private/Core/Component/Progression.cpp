// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/Progression.h"

#include "Net/UnrealNetwork.h"

UProgression::UProgression()
{
	PrimaryComponentTick.bCanEverTick = false;

	// server ready
	SetIsReplicatedByDefault(true);
	
	// Default Status Values
	StatusConfigs = {
		 {EStatusType::STAP, FText::FromString(TEXT("STAP")), 2, 2},
		 {EStatusType::HACK, FText::FromString(TEXT("HACK")), 4, 1},
		 {EStatusType::INT, FText::FromString(TEXT("INT")), 1, 3},
		 {EStatusType::DEF, FText::FromString(TEXT("DEF")), 2, 2},
		 {EStatusType::MR, FText::FromString(TEXT("MR")), 1, 3},
		 {EStatusType::DEX, FText::FromString(TEXT("DEX")), 3, 1},
		 {EStatusType::AGI, FText::FromString(TEXT("AGI")), 2, 2},
	};
	// Sets size of array
	StatusValues.SetNum(StatusConfigs.Num());
}


// Called when the game starts
void UProgression::BeginPlay()
{
	Super::BeginPlay();
	InitDefaultsIfAuthority();
}

void UProgression::InitDefaultsIfAuthority()
{
	// Server initializes and replicates (if single, locally)
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}
	Level = 1;
	UnspentPoints = 0;
	
	StatusValues.SetNum(StatusConfigs.Num());
	for (int32 i = 0; i < StatusConfigs.Num(); ++i)
	{
		StatusValues[i] = StatusConfigs[i].BaseValue;
	}
	OnProgressionChanged.Broadcast();
}

int32 UProgression::FindConfigIndex(EStatusType Type) const
{
	for (int32 i = 0; i < StatusConfigs.Num(); ++i)
	{
		if (StatusConfigs[i].Type == Type)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UProgression::GetStatusValue(EStatusType Type) const
{
	const int32 Idx = FindConfigIndex(Type);
	return (Idx != INDEX_NONE && StatusValues.IsValidIndex(Idx)) ? StatusValues[Idx] : 0;
}

FStatusConfig UProgression::GetStatusConfig(EStatusType Type) const
{
	const int32 Idx = FindConfigIndex(Type);
	return (Idx != INDEX_NONE) ? StatusConfigs[Idx] : FStatusConfig{};
}

TArray<FStatusViewData> UProgression::GetAllStatusViewData() const
{
	TArray<FStatusViewData> Out;
	Out.Reserve(StatusConfigs.Num());

	for (int i = 0; i < StatusConfigs.Num(); ++i)
	{
		FStatusViewData V;
		V.Type = StatusConfigs[i].Type;
		V.DisplayName = StatusConfigs[i].DisplayName;
		V.CostPerIncrease = StatusConfigs[i].CostPerIncrease;
		V.Value = StatusValues.IsValidIndex(i) ? StatusValues[i] : 0;
		Out.Add(V);
	}
	return Out;
}

void UProgression::LevelUp()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	// if client, requests
	if (!Owner->HasAuthority())
	{
		ServerLevelUp();
		return;
	}
	Level += 1;
	UnspentPoints += PointsPerLevelUp;
	
	OnProgressionChanged.Broadcast();
}

bool UProgression::TryIncreaseStatus(EStatusType Type)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	// if client, requests
	if (!Owner->HasAuthority())
	{
		ServerTryIncreaseStatus(Type);
		return false;
	}
	
	const int32 Idx = FindConfigIndex(Type);
	if (Idx == INDEX_NONE)
	{
		return false;
	}
	const int32 Cost = StatusConfigs[Idx].CostPerIncrease;
	if (UnspentPoints < Cost)
	{
		return  false;
	}
	
	UnspentPoints -= Cost;
	StatusValues[Idx] += 1;
	
	OnProgressionChanged.Broadcast();
	return true;
}

void UProgression::OnRep_Progression()
{
	// Refreshes UI if value refreshed
	OnProgressionChanged.Broadcast();
}

void UProgression::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UProgression, Level);
	DOREPLIFETIME(UProgression, UnspentPoints);
	DOREPLIFETIME(UProgression, StatusValues);
}

// Server RPC
void UProgression::ServerLevelUp_Implementation()
{
	LevelUp();
}

void UProgression::ServerTryIncreaseStatus_Implementation(EStatusType Type)
{
	TryIncreaseStatus(Type);
}
