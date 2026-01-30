// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatUIChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	// Starts attacking monster targeted by PlayerController
	UFUNCTION(BlueprintCallable, Category="Combat")
	void TryBasicAttack(AEnemy* Target);
	UFUNCTION(BlueprintCallable, Category="Combat")
	void NotifyAttackHit();
	// For UI
	UFUNCTION(BlueprintPure, Category="Combat|UI")
	int32 GetCurrentCombo() const { return CurrentCombo; }
	UFUNCTION(BlueprintPure, Category="Combat|UI")
	int32 GetMaxCombo() const { return MaxCombo; }
	UFUNCTION(BlueprintPure, Category="Combat|UI")
	float GetComboWindowPercent() const;
	UFUNCTION(BlueprintPure, Category="Combat|UI")
	bool IsComboWindowOpen() const { return bComboWindowOpen; }
	
	UPROPERTY(BlueprintAssignable, Category="Combat|UI")
	FOnCombatUIChanged OnCombatUIChanged;
	// Requests animation to play (Blueprint)

	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRange = 200.f;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	int32 BasicAttackDamage = 10;
	UPROPERTY(VisibleAnywhere, Category="Combat|Combo")
	int32 MaxCombo = 1;
	UPROPERTY(VisibleAnywhere, Category="Combat|Combo")
	int32 CurrentCombo = 0;
	
	// Combo Window
	UPROPERTY(EditDefaultsOnly, Category="Combat|Combo")
	float ComboWindowDuration = 1.f;
	
	bool bComboWindowOpen = false;
	float ComboWindowEndTime = 0.f;
	
	// Attack lock (prevents spamming attack)
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackLockTime = 1.45f;
	
	bool bAttackLocked = false;
	
	// Current Target
	UPROPERTY()
	TObjectPtr<AEnemy> CurrentTarget;
	
	FTimerHandle Timer_AttackLock;
	FTimerHandle Timer_CloseWindow;
	
	void UnlockAttack();
	void CloseComboWindow();
	void OpenComboWindow();
};
