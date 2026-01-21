// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/Vital.h"

#include "Core/Component/Progression.h"
#include "Net/UnrealNetwork.h"

UVital::UVital()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	// Initial Values
	ResourceConfigs = {
		{EResourceType::HP, FText::FromString(TEXT("HP")), 220, 20, true},
		{EResourceType::MP, FText::FromString(TEXT("MP")), 50, 5, true},
		{EResourceType::SP, FText::FromString(TEXT("HP")), 1000, 50, true},
	};
	
	ResourceStates.SetNum(ResourceConfigs.Num());
}


// Called when the game starts
void UVital::BeginPlay()
{
	Super::BeginPlay();
	InitDefaultsIfAuthority();
	
	// Subscribes LevelUp Event
	if (AActor* Owner = GetOwner())
	{
		if (UProgression* Progression = Owner->FindComponentByClass<UProgression>())
		{
			Progression->OnLevelUp.AddDynamic(this, &UVital::HandleLevelUp);
		}
	}
}

void UVital::InitDefaultsIfAuthority()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}
	
	ResourceStates.SetNum(ResourceConfigs.Num());
	for (int32 i = 0; i < ResourceConfigs.Num(); ++i)
	{
		ResourceStates[i].Type = ResourceConfigs[i].Type;
		ResourceStates[i].Max = ResourceConfigs[i].BaseMax;
		ResourceStates[i].Current = ResourceConfigs[i].BaseMax;
	}
	OnVitalChanged.Broadcast();
}

int32 UVital::FindIndex(EResourceType Type) const
{
	for (int32 i = 0; i < ResourceStates.Num(); ++i)
	{
		if (ResourceStates[i].Type == Type)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UVital::ClampCurrent(int32 Index)
{
	if (!ResourceStates.IsValidIndex(Index))
	{
		return;
	}
	ResourceStates[Index].Current = FMath::Clamp(ResourceStates[Index].Current, 0, ResourceStates[Index].Max);
}

FResourceState UVital::GetResource(EResourceType Type) const
{
	const int32 Idx = FindIndex(Type);
	return (Idx != INDEX_NONE) ? ResourceStates[Idx] : FResourceState{};
}

TArray<FResourceViewData> UVital::GetAllResourceViewData() const
{
	TArray<FResourceViewData> Out;
	Out.Reserve(ResourceConfigs.Num());
	
	for (int32 i = 0; i < ResourceConfigs.Num(); ++i)
	{
		FResourceViewData V;
		V.Type = ResourceConfigs[i].Type;
		V.DisplayName = ResourceConfigs[i].DisplayName;
		V.MaxIncreasePerLevel = ResourceConfigs[i].MaxIncreasePerLevel;
		
		const int32 StateIdx = FindIndex(ResourceConfigs[i].Type);
		if (StateIdx != INDEX_NONE)
		{
			V.Current = ResourceStates[StateIdx].Current;
			V.Max = ResourceStates[StateIdx].Max;
		}
		Out.Add(V);
	}
	return Out;
}

void UVital::ApplyDelta(EResourceType Type, int32 Delta)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	if (!Owner->HasAuthority())
	{
		return;
	}
	
	const int32 Idx = FindIndex(Type);
	if (Idx == INDEX_NONE)
	{
		return;
	}
	
	ResourceStates[Idx].Current += Delta;
	ClampCurrent(Idx);
	
	OnVitalChanged.Broadcast();
}

void UVital::HandleLevelUp(int32 NewLevel)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;;
	}
	
	for (int32 i = 0; i < ResourceConfigs.Num(); ++i)
	{
		const EResourceType Type = ResourceConfigs[i].Type;
		const int32 StateIdx = FindIndex(Type);
		if (StateIdx == INDEX_NONE)
		{
			continue;
		}
		const int32 Inc = ResourceConfigs[i].MaxIncreasePerLevel;
		ResourceStates[StateIdx].Max += Inc;
		
		if (ResourceConfigs[i].bIncreaseCurrentOnLevelUp)
		{
			ResourceStates[StateIdx].Current += Inc;
		}
		ClampCurrent(StateIdx);
	}
	OnVitalChanged.Broadcast();
}

void UVital::OnRep_Vital()
{
	OnVitalChanged.Broadcast();
}

void UVital::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVital, ResourceStates);
}
