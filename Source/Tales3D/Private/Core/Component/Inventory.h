// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

// Item slot struct
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemId = NAME_None;
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
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRepItems)
	TArray<FInventoryItem> Items;
	
	UFUNCTION()
	void OnRep_Items();
	
	// Server Ready
	UFUNCTION(Server, Reliable)
	void ServerAddItem(FName ItemId, int32 Amount);
	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(FName ItemId, int32 Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	int32 FindIndex(FName ItemId) const;

public:	
	// Item List(Read)
	UFUNCTION(BlueprintPure, Category="Inventory")
	const TArray<FInventoryItem>& GetItems() const { return Items; }
	// Item Add/Remove
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItem(FName ItemId, int32 Amount); 
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(FName ItemId, int32 Amount);
	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetQuantity(FName ItemId) const;
	// For UI Binding
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryChanged OnInventoryChanged;
};
