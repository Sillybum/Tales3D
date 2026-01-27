// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/Equipment.h"

#include "Progression.h"
#include "Core/Char/Human.h"
#include "Core/Data/WeaponItemData.h"
#include "Engine/AssetManager.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UEquipment::UEquipment()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UEquipment::BeginPlay()
{
	Super::BeginPlay();
}

void UEquipment::EquipWeapon_Internal(FPrimaryAssetId WeaponPrimaryId)
{
	if (!WeaponPrimaryId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Equip] Invalid PrimaryAssetId"));
		return;
	}

	UAssetManager& AM = UAssetManager::Get();
	const FSoftObjectPath Path = AM.GetPrimaryAssetPath(WeaponPrimaryId);
	if (!Path.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Equip] No PrimaryAssetPath for %s"), *WeaponPrimaryId.ToString());
		return;
	}

	UWeaponItemData* WeaponData = Cast<UWeaponItemData>(Path.TryLoad());
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Equip] Load failed: %s"), *Path.ToString());
		return;
	}

	if (!CanEquip(WeaponData))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Equip] CanEquip failed (Level/Stats/CharacterType)"));
		return;
	}

	SpawnAndAttachWeapon(WeaponData);
	UE_LOG(LogTemp, Warning, TEXT("[Equip] SpawnAndAttach called"));
	EquippedWeaponId = WeaponPrimaryId;
	OnEquipmentChanged.Broadcast();
}


void UEquipment::SpawnAndAttachWeapon(const UWeaponItemData* WeaponData)
{
	if (!WeaponData)
	{
		return;
	}
	if (!WeaponData->EquippedActorClass.IsValid() && WeaponData->EquippedActorClass.ToSoftObjectPath().IsNull())
	{
		return;
	}
	
	AActor* Owner = GetOwner();
	ACharacter* Char = Cast<ACharacter>(Owner);
	if (!Char)
	{
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Char->GetMesh();
	if (!MeshComp)
	{
		return;
	}
	
	// Loads BP class and Spawns
	UClass* WeaponClass = WeaponData->EquippedActorClass.LoadSynchronous();
	if (!WeaponClass)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Char;
	
	AActor* Spawned = World->SpawnActor<AActor>(WeaponClass, Params);
	if (!Spawned) return;
	
	Spawned->SetActorEnableCollision(false);
	
	// Server Ready (Replicated if spawned on server)
	Spawned->SetReplicates(true);
	// Attaches to socket
	Spawned->AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponData->AttachSocketName
		);
	
	EquippedWeaponActor = Spawned;
}

bool UEquipment::CanEquip(const UWeaponItemData* WeaponData) const
{
	if (!WeaponData)
	{
		return false;
	}
	
	const AActor* Owner = GetOwner();
	const AHuman* Human	= Cast<AHuman>(Owner);
	const UProgression* Progression = Owner ? Owner->FindComponentByClass<UProgression>() : nullptr;
	
	// Checks Character Type
	if (Human)
	{
		if (WeaponData->AllowedCharacters.Num() > 0 && !WeaponData->AllowedCharacters.Contains(Human->CharacterType))
		{
			return false;
		}
	}
	
	// Checks Level Requirement
	if (Progression && Progression->GetLevel() < WeaponData->RequiredLevel)
	{
		return false;
	}
	
	// Checks STAB/HACK Requirement
	if (Progression)
	{
		const int32 STAB = Progression->GetStatusValue(EStatusType::STAB);
		const int32 HACK = Progression->GetStatusValue(EStatusType::HACK);
		
		if (STAB < WeaponData->RequiredStab)
		{
			return false;
		}
		if (HACK < WeaponData->RequiredHack)
		{
			return false;
		}
	}
	return true;
}

void UEquipment::OnRep_Equipped()
{
	OnEquipmentChanged.Broadcast();
}

void UEquipment::ServerEquipWeaponByPrimaryId_Implementation(FPrimaryAssetId WeaponPrimaryId)
{
	EquipWeapon_Internal(WeaponPrimaryId);
}

void UEquipment::ServerUnequipWeapon_Implementation()
{
	UnequipWeapon();
}

void UEquipment::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UEquipment, EquippedWeaponId);
	DOREPLIFETIME(UEquipment, EquippedWeaponActor);
}

void UEquipment::EquipWeaponByPrimaryId(FPrimaryAssetId WeaponPrimaryId)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	// request to server if client
	if (!Owner->HasAuthority())
	{
		ServerEquipWeaponByPrimaryId(WeaponPrimaryId);
		return;
	}
	EquipWeapon_Internal(WeaponPrimaryId);		
}

void UEquipment::UnequipWeapon()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	if (!Owner->HasAuthority())
	{
		ServerUnequipWeapon();
		return;
	}
	
	// Removes Equipped
	if (EquippedWeaponActor)
	{
		EquippedWeaponActor->Destroy();
		EquippedWeaponActor = nullptr;
	}
	
	EquippedWeaponId = FPrimaryAssetId();
	OnEquipmentChanged.Broadcast();
}

