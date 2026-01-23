// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/Inventory.h"

#include "Core/Data/ItemData.h"
#include "Engine/AssetManager.h"
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

int32 UInventory::FindIndex(FPrimaryAssetId PrimaryId) const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].PrimaryId == PrimaryId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UInventory::AddItemByName(FName ItemName, int32 Amount)
{
	if (ItemName.IsNone())
	{
		return;
	}
	AddItemByPrimaryId(FPrimaryAssetId(UItemData::AssetTypeName, ItemName), Amount);
}

void UInventory::AddItemByPrimaryId(FPrimaryAssetId PrimaryId, int32 Amount)
{
	if (!PrimaryId.IsValid() || Amount <= 0)
	{
		return;
	}
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	const int32 Idx = FindIndex(PrimaryId);
	if (Idx == INDEX_NONE)
	{
		FInventoryItem NewItem;
		NewItem.PrimaryId = PrimaryId;
		NewItem.Quantity = Amount;
		Items.Add(NewItem);
	}
	else
	{
		Items[Idx].Quantity += Amount;
	}
	OnInventoryChanged.Broadcast();
}

int32 UInventory::GetQuantityByPrimaryId(FPrimaryAssetId PrimaryId) const
{
	const int32 Idx = FindIndex(PrimaryId);
	return (Idx != INDEX_NONE) ? Items[Idx].Quantity : 0;
}

// Sync
UItemData* UInventory::GetItemDataSync(FPrimaryAssetId PrimaryId) const
{
	if (!PrimaryId.IsValid())
	{
		return nullptr;
	}
	
	UAssetManager& AM = UAssetManager::Get();
	
	if (UObject* Obj = AM.GetPrimaryAssetObject(PrimaryId))
	{
		return Cast<UItemData>(Obj);
	}
	
	const FSoftObjectPath Path = AM.GetPrimaryAssetPath(PrimaryId);
	if (!Path.IsValid())
	{
		return nullptr;
	}
	
	return Cast<UItemData>(Path.TryLoad());
}

void UInventory::RequestItemDataAsync(FPrimaryAssetId PrimaryId, TFunction<void(class UItemData* Data)> OnLoaded)
{
	if (!PrimaryId.IsValid())
	{
		OnLoaded(nullptr);
		return;
	}
	
	UAssetManager& AM = UAssetManager::Get();
	
	if (UObject* Obj = AM.GetPrimaryAssetObject(PrimaryId))
	{
		OnLoaded(Cast<UItemData>(Obj));
		return;
	}
	TArray<FPrimaryAssetId> Ids;
	Ids.Add(PrimaryId);
	TArray<FName> Bundles;
	AM.LoadPrimaryAssets(
		Ids,
		Bundles,
		FStreamableDelegate::CreateLambda([PrimaryId, OnLoaded]()
		{
			UAssetManager& AM2 = UAssetManager::Get();
			UItemData* Data = Cast<UItemData>(AM2.GetPrimaryAssetObject(PrimaryId));
			OnLoaded(Data);
		})
		);
}

void UInventory::RequestIconAsync(TSoftObjectPtr<class UTexture2D> Icon, TFunction<void(UTexture2D* Tex)> OnLoaded)
{
	if (Icon.IsNull())
	{
		OnLoaded(nullptr);
		return;
	}
	
	// if already loaded
	if (UTexture2D* Already = Icon.Get())
	{
		OnLoaded(Already);
		return;
	}
	
	FStreamableManager& SM = UAssetManager::GetStreamableManager();
	const FSoftObjectPath Path = Icon.ToSoftObjectPath();
	
	SM.RequestAsyncLoad(Path, FStreamableDelegate::CreateLambda([Icon, OnLoaded]()
	{
		OnLoaded(Icon.Get());
	}));
}

bool UInventory::RemoveItem(FPrimaryAssetId PrimaryId, int32 Amount)
{
	if (!PrimaryId.IsValid() || Amount <= 0)
	{
		return false;
	}
	
	const int32 Idx = FindIndex(PrimaryId);
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

void UInventory::ServerAddItem_Implementation(FName ItemId, int32 Amount)
{
	AddItemByName(ItemId, Amount);
}

void UInventory::ServerRemoveItem_Implementation(FPrimaryAssetId FPrimaryId, int32 Amount)
{
	RemoveItem(FPrimaryId, Amount);
}
