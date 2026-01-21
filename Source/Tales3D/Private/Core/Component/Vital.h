// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Vital.generated.h"

// Custom Components
class UProgression;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	HP	UMETA(DisplayName="HP"),
	MP	UMETA(DisplayName="MP"),
	SP	UMETA(DisplayName="SP"),
};
// Values on UI
USTRUCT(BlueprintType)
struct FResourceConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EResourceType Type = EResourceType::HP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	// Lv1 Max Value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BaseMax = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxIncreasePerLevel = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIncreaseCurrentOnLevelUp = true;
};
// Runtime State (can replicate)
USTRUCT(BlueprintType)
struct FResourceState
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EResourceType Type = EResourceType::HP;
	UPROPERTY(BlueprintReadOnly)
	int32 Current = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 Max = 0;
};
// ViewData for UI
USTRUCT(BlueprintType)
struct FResourceViewData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EResourceType Type = EResourceType::HP;
	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(BlueprintReadOnly)
	int32 Current = 0;
	UPROPERTY(BlueprintReadOnly)	
	int32 Max = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxIncreasePerLevel = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVitalChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UVital : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVital();

protected:
	virtual void BeginPlay() override;
public:	
	UFUNCTION(BlueprintPure, Category="Vital")
	FResourceState GetResource(EResourceType Type) const;	
	UFUNCTION(BlueprintPure, Category="Vital")
	TArray<FResourceViewData> GetAllResourceViewData() const;
	// For gameplay
	UFUNCTION(BlueprintCallable, Category="Vital")
	void ApplyDelta(EResourceType Type, int32 Delta);
	UPROPERTY(BlueprintAssignable, Category="Vital")
	FOnVitalChanged OnVitalChanged;
	
protected:
	// Default Values & LevelUp Increases
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Vital")
	TArray<FResourceConfig> ResourceConfigs;
	UPROPERTY(ReplicatedUsing=OnRep_Vital)
	TArray<FResourceState> ResourceStates;
	
	UFUNCTION()
	void OnRep_Vital();
	// Handler for Progression LevelUp Event
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	int32 FindIndex(EResourceType Type) const;
	void InitDefaultsIfAuthority();
	void ClampCurrent(int32 Index);
};
