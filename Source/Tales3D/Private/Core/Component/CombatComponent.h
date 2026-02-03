// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatStateChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	
	// Processes Input (Called from BP)
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	bool CanStartBasicAttack(AEnemy* Target) const;
	// Starts first combo (Initialize)
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	void BeginBasicAttack(AEnemy* Target, int32 InMaxCombo);
	// Input Buffer
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	void QueueBasicInput();
	// Checks if can cancel basic attack
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	bool TryConsumeInputAndAdvance();
	// opens/closes window (called from notify)
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	void SetComboWindowOpen(bool bOpen);
	// Hit Detection (called from notify)
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	void NotifyAttackHit();
	// Called when montage exits
	UFUNCTION(BlueprintCallable, Category="Combat|Basic")
	void EndBasicAttack();
	
	// ----------------------------
	// Values read by BP/UI
	// ----------------------------
	UFUNCTION(BlueprintPure, Category="Combat|Combo")
	bool IsComboWindowOpen() const { return bComboWindowOpen; }
	UFUNCTION(BlueprintPure, Category="Combat|Combo")
	int32 GetCurrentCombo() const { return CurrentCombo; }
	UFUNCTION(BlueprintPure, Category="Combat|Combo")
	int32 GetMaxCombo() const { return MaxCombo; }
	UFUNCTION(BlueprintPure, Category="Combat|Combo")
	FName GetCurrentComboSection() const;
	UFUNCTION(BlueprintPure, Category="Combat|Combo")
	AEnemy* GetCurrentTarget() const { return CurrentTarget; }
	UPROPERTY(BlueprintAssignable, Category="Combat|UI")
	FOnCombatStateChanged OnCombatStateChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRange = 250.f;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	int32 BasicAttackDamage = 10;
	
private:
	UPROPERTY()
	TObjectPtr<AEnemy> CurrentTarget;
	
	int32 CurrentCombo = 0;
	int32 MaxCombo = 5;
	
	bool bComboWindowOpen = false;
	bool bInputQueued = false;
	
	// prevents duplicated hit (cases where somehow two notifies happen)
	int32 LastHitComboIndex = 0;
	
	bool CanAdvance() const;
	void AdvanceCombo_Internal();
};
