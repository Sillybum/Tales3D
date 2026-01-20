// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/AssetManagerTypes.h"
#include "Inventory.generated.h"

class UItemData;

// Item slot struct
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId PrimaryId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Items)
	TArray<FInventoryItem> Items;
	
	UFUNCTION()
	void OnRep_Items();
	
	// Server Ready
	UFUNCTION(Server, Reliable)
	void ServerAddItem(FName ItemId, int32 Amount);
	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(FPrimaryAssetId PrimaryId, int32 Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	int32 FindIndex(FPrimaryAssetId PrimaryId) const;

public:	
	// Item List(Read)
	UFUNCTION(BlueprintPure, Category="Inventory")
	const TArray<FInventoryItem>& GetItems() const { return Items; }
	// Item Add/Remove
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItemByName(FName ItemName, int32 Amount); 
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItemByPrimaryId(FPrimaryAssetId PrimaryId, int32 Amount);
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(FPrimaryAssetId PrimaryId, int32 Amount);
	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetQuantityByPrimaryId(FPrimaryAssetId PrimaryId) const;
	// For UI Binding
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryChanged OnInventoryChanged;
	
	// async load
	void RequestItemDataAsync(FPrimaryAssetId PrimaryId, TFunction<void(class UItemData* Data)> OnLoaded);
	void RequestIconAsync(TSoftObjectPtr<class UTexture2D> Icon, TFunction<void(UTexture2D* Tex)> OnLoaded);
	
	
};
