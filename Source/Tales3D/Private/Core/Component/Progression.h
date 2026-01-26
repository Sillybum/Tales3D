// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression.generated.h"

// 스탯을 순서대로 다루기 좋게
UENUM(BlueprintType)
enum class EStatusType : uint8
{
	STAB	UMETA(DisplayName="STAB"),
	HACK	UMETA(DisplayName="HACK"),
	INT		UMETA(DisplayName="INT"),
	DEF		UMETA(DisplayName="DEF"),
	MR		UMETA(DisplayName="MR"),
	DEX		UMETA(DisplayName="DEX"),
	AGI		UMETA(DisplayName="AGI"),
};

USTRUCT(BlueprintType)
struct FStatusConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EStatusType Type = EStatusType::STAB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BaseValue = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 CostPerIncrease = 1;
};

// For UI use
USTRUCT(BlueprintType)
struct FStatusViewData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EStatusType Type = EStatusType::STAB;
	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(BlueprintReadOnly)
	int32 Value = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 CostPerIncrease = 0;
};

// Refreshs widget
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProgressionChanged);
// LevelUp Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UProgression : public UActorComponent
{
	GENERATED_BODY()

public:	
	UProgression();


protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Progression")
	int32 PointsPerLevelUp = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Progression")
	TArray<FStatusConfig> StatusConfigs;
	
	// values be replicated
	UPROPERTY(ReplicatedUsing=OnRep_Progression)
	int32 Level = 1;
	UPROPERTY(ReplicatedUsing=OnRep_Progression)
	int32 UnspentPoints = 0;
	// Actual values be replicated
	UPROPERTY(ReplicatedUsing=OnRep_Progression)
	TArray<int32> StatusValues;
	
	UFUNCTION()
	void OnRep_Progression();
	
	// server ready
	UFUNCTION(Server, Reliable)
	void ServerLevelUp();
	UFUNCTION(Server, Reliable)
	void ServerTryIncreaseStatus(EStatusType Type);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// Values be read from UI
	UFUNCTION(BlueprintPure, Category="Progression")
	int32 GetLevel() const { return Level; }
	UFUNCTION(BlueprintPure, Category="Progression")
	int32 GetUnspentPoints() const { return UnspentPoints; }
	UFUNCTION(BlueprintPure, Category="Progression")
	int32 GetPointsPerLevelUp() const { return PointsPerLevelUp; }
	UFUNCTION(BlueprintPure, Category="Progression")
	int32 GetStatusValue(EStatusType Type) const;
	UFUNCTION(BlueprintPure, Category="Progression")
	FStatusConfig GetStatusConfig(EStatusType Type) const;
	UFUNCTION(BlueprintPure, Category="Progression")
	TArray<FStatusViewData> GetAllStatusViewData() const;
	
	// features
	UFUNCTION(BlueprintCallable, Category="Progression")
	void LevelUp();
	UFUNCTION(BlueprintCallable, Category="Progression")
	bool TryIncreaseStatus(EStatusType Type);
	
	// UI Refresh Signal
	UPROPERTY(BlueprintAssignable, Category="Progression")
	FOnProgressionChanged OnProgressionChanged;
	// LevelUp Event
	UPROPERTY(BlueprintAssignable, Category="Progression")
	FOnLevelUp OnLevelUp;
	
private:
	int32 FindConfigIndex(EStatusType Type) const;
	void InitDefaultsIfAuthority();
		
};
