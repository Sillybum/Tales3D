// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "CombatComponent.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

public:
	// BasicAttacks monster selected
	UFUNCTION(BlueprintCallable, Category="Combat")
	void StartBasicAttack(AActor* TargetActor);
	// Actual Hit Processing
	UFUNCTION(BlueprintCallable, Category="Combat")
	void Notify_BasicAttackHit();
	
protected:
	// Basic Attack Montage that be assigned in BP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Basic")
	TObjectPtr<UAnimMontage> BasicAttackMontage;
	// Montage Section Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Basic")
	TArray<FName> ComboSections = { TEXT("A1"), TEXT("A2"), TEXT("A3"), TEXT("A4"), TEXT("A5")};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Basic")
	int32 DamagePerHit = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Basic")
	float AttackRange = 180.f;
	// prolonging time after finishing attack (for skill combo)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Basic")
	float ComboGraceTime = 0.8f;
	
private:
	TWeakObjectPtr<AActor> CurrentTarget;
	
	bool bIsAttacking = false;
	bool bQueueNextAttack = false;
	
	int32 ComboStep = 1;
	int32 CachedMaxCombo = 1;
	
	FTimerHandle ComboResetTimer;
	
	int32 GetMaxBasicComboFromLevel() const;
	void PlayComboStep();
	void ResetCombo();
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION(BlueprintCallable, Category="Combat")
	void Notify_BasicAttackEnd();
};

inline void UCombatComponent::Notify_BasicAttackEnd()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar || !BasicAttackMontage)
	{
		return;
	}
	// Stops Montage so that OnMontageEnded be called
	OwnerChar->StopAnimMontage(BasicAttackMontage);
}
