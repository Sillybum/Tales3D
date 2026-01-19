// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/Inventory.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	// Auto Replication
	SetIsReplicatedByDefault(true);

	// ...
}

void UInventory::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventory, Items);
}

void UInventory::OnRep_Items()
{
	// Triggers UI Refresh
	OnInventoryChanged.Broadcast();
}

int32 UInventory::FindIndex(FName ItemId) const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].ItemId == ItemId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

int32 UInventory::GetQuantity(FName ItemId) const
{
	const int32 Idx = FindIndex(ItemId);
	return (Idx != INDEX_NONE) ? Items[Idx].Quantity : 0;
}

void UInventory::AddItem(FName ItemId, int32 Amount)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return;
	}
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerAddItem(ItemId, Amount);
		return;
	}
	
	const int32 Idx = FindIndex(ItemId);
	if (Idx == INDEX_NONE)
	{
		FInventoryItem NewItem;
		NewItem.ItemId = ItemId;
		NewItem.Quantity = Amount;
		Items.Add(NewItem);
	}
	else
	{
		Items[Idx].Quantity += Amount;
	}
	
	OnInventoryChanged.Broadcast();
}

bool UInventory::RemoveItem(FName ItemId, int32 Amount)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return false;
	}
	
	const int32 Idx = FindIndex(ItemId);
	if (Idx == INDEX_NONE)
	{
		return false;
	}
	if (Items[Idx].Quantity < Amount)
	{
		return false;
	}
	Items[Idx].Quantity -= Amount;
	
	// Removes from slot if 0
	if (Items[Idx].Quantity == 0)
	{
		Items.RemoveAt(Idx);
	}
	
	OnInventoryChanged.Broadcast();
	return true;
}
