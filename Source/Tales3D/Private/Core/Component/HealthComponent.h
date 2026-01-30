// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, int32, Current, int32, Max);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_Health, Category="Health")
	int32 Max = 100;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_Health, Category="Health")
	int32 Current = 100;
	
	UFUNCTION()
	void OnRep_Health();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UFUNCTION(BlueprintPure, Category="Health")
	int32 GetCurrent() const { return Current; }
	UFUNCTION(BlueprintPure, Category="Health")
	int32 GetMax() const { return Max; }
	UFUNCTION(BlueprintPure, Category="Health")
	float GetPercent() const { return (Max > 0) ? (float)Current / (float)Max : 0.f; }
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetMaxAndFill(int32 NewMax);
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChanged OnHealthChanged;
	UFUNCTION(BlueprintCallable, Category="Health")
	void ApplyDamage(int32 Amount);
};
