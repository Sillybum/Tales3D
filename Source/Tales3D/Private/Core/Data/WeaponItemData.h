// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Core/Data/ItemData.h"
#include "WeaponItemData.generated.h"

UENUM(BlueprintType)
enum class EPlayableCharacter : uint8
{
	Maximin		UMETA(DisplayName="막시민 리프크네"),
	Boris		UMETA(DisplayName="보리스 진네만"),
};
/**
 * 
 */
UCLASS()
class UWeaponItemData : public UItemData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 DurabilityMax = 21;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 DurabilityCurrent = 21;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Hardness = 24;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 StabAttack = 24;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 HackAttack = 36;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 PhysicalDefense = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 MagicAttack = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Accuracy = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Evasion = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Agility = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Critical = 2;
	// Clamps % value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(ClampMin="0", ClampMax="150"))
	int32 WeaponDelayPercent = 95;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 Weight = 33;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	int32 SynthesisCount = 2;
	
	// Requirement for equipment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Requirement")
	TArray<EPlayableCharacter> AllowedCharacters;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Requirement")
	int32 RequiredLevel = 24;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Requirement")
	int32 RequiredStab = 13;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Requirement")
	int32 RequiredHack = 27;
	
	// For actual equipping
	// Which actor(Weapon) to equip
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Visual")
	TSoftClassPtr<AActor> EquippedActorClass;
	// Which socket to use to equip(Skeletal socket name)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Visual")
	FName AttachSocketName = TEXT("weapon_r");
	
	// Overrides parent's function
	virtual void GetDetailLines_Implementation(TArray<FText>& OutLines) const override;
};
