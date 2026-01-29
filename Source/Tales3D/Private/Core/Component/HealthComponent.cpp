// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/HealthComponent.h"

#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(Current, Max);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Max);
	DOREPLIFETIME(UHealthComponent, Current);
}

void UHealthComponent::SetMaxAndFill(int32 NewMax)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	Max = FMath::Max(1, NewMax);
	Current = Max;
	OnHealthChanged.Broadcast(Current, Max);
}

// Combat
void UHealthComponent::ApplyDamage(int32 Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	const int32 Dmg = FMath::Max(0, Amount);
	Current = FMath::Clamp(Current - Dmg, 0, Max);
	
	OnHealthChanged.Broadcast(Current, Max);
}
