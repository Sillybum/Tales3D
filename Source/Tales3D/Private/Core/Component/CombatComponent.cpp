// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/CombatComponent.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/SPIRV-Headers/include/spirv/unified1/spirv.h>

#include "HealthComponent.h"
#include "Progression.h"
#include "Core/Char/Enemy.h"
#include "Core/Char/Human.h"


bool UCombatComponent::CanStartBasicAttack(AEnemy* Target) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return false;
	}
	
	const float Dist = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());
	return Dist <= AttackRange;
}

void UCombatComponent::BeginBasicAttack(AEnemy* Target, int32 InMaxCombo)
{
	CurrentTarget = Target;
	MaxCombo = FMath::Max(1, InMaxCombo);
	CurrentCombo = 1;
	
	// Window be opened by notify
	bComboWindowOpen = false;
	// Initializes
	bInputQueued = false;
	LastHitComboIndex = 0;
	
	OnCombatStateChanged.Broadcast();
}

void UCombatComponent::QueueBasicInput()
{
	bInputQueued = true;
	OnCombatStateChanged.Broadcast();
}

bool UCombatComponent::TryConsumeInputAndAdvance()
{
	// While Window && Input queued
	if (!bComboWindowOpen)
	{
		UE_LOG(LogTemp,Warning, TEXT("TryConsumeInputAndAdvance: Window Closed"));
		return false;
	}
	if (!bInputQueued)
	{
		UE_LOG(LogTemp,Warning, TEXT("TryConsumeInputAndAdvance: No queue"));
		return false;
	}
	if (!CanAdvance())
	{
		UE_LOG(LogTemp,Warning, TEXT("TryConsumeInputAndAdvance: Can't advance"));
		return false;
	}
	AdvanceCombo_Internal();
	return true;
}

void UCombatComponent::SetComboWindowOpen(bool bOpen)
{
	bComboWindowOpen = bOpen;
	OnCombatStateChanged.Broadcast();
}

void UCombatComponent::NotifyAttackHit()
{
	if (!CurrentTarget) return;
	if (CurrentCombo <= 0) return;
	
	// prevents duplicated hit
	if (LastHitComboIndex == CurrentCombo) return;
	LastHitComboIndex = CurrentCombo;

	if (UHealthComponent* H = CurrentTarget->FindComponentByClass<UHealthComponent>())
	{
		H->ApplyDamage(BasicAttackDamage);
	}
}

void UCombatComponent::EndBasicAttack()
{
	CurrentTarget = nullptr;
	CurrentCombo = 0;
	MaxCombo = 1;
	bComboWindowOpen = false;
	bInputQueued = false;
	LastHitComboIndex = 0;
	
	OnCombatStateChanged.Broadcast();
}

FName UCombatComponent::GetCurrentComboSection() const
{
	// A1, A2, A3...
	if (CurrentCombo <= 0) return NAME_None;
	
	const FString Name = FString::Printf(TEXT("A%d"), CurrentCombo);
	return FName(*Name);
}

bool UCombatComponent::CanAdvance() const
{
	return (CurrentCombo > 0) && (CurrentCombo < MaxCombo);
}

void UCombatComponent::AdvanceCombo_Internal()
{
	if (!CanAdvance()) return;
	
	CurrentCombo += 1;
	bComboWindowOpen = false;
	bInputQueued = false;
	OnCombatStateChanged.Broadcast();
}

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
