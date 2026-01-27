// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equipment.generated.h"

class UWeaponItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEquipment : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipment();

protected:
	virtual void BeginPlay() override;

private:
	// Inner
	void EquipWeapon_Internal(FPrimaryAssetId WeaponPrimaryId);
	void SpawnAndAttachWeapon(const UWeaponItemData* WeaponData);
	bool CanEquip(const UWeaponItemData* WeaponData) const;
	
protected:
	// Id for current weapon
	UPROPERTY(ReplicatedUsing=OnRep_Equipped)
	FPrimaryAssetId EquippedWeaponId;
	UPROPERTY(Replicated)
	TObjectPtr<AActor> EquippedWeaponActor;
	UFUNCTION()
	void OnRep_Equipped();
	// Server Ready
	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponByPrimaryId(FPrimaryAssetId WeaponPrimaryId);
	UFUNCTION(Server, Reliable)
	void ServerUnequipWeapon();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void EquipWeaponByPrimaryId(FPrimaryAssetId WeaponPrimaryId);
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void UnequipWeapon();
	
	UFUNCTION(BlueprintPure, Category="Equipment")
	bool HasEquippedWeapon() const { return EquippedWeaponId.IsValid(); }
	UFUNCTION(BlueprintPure, Category="Equipment")
	FPrimaryAssetId GetEquippdeWeaponId() const { return EquippedWeaponId; }	
	UPROPERTY(BlueprintAssignable, Category="Equipment")
	FOnEquipmentChanged OnEquipmentChanged;
};
