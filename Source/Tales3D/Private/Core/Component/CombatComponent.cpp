// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/CombatComponent.h"

#include "HealthComponent.h"
#include "Core/Char/Enemy.h"
#include "Core/Char/Human.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::TryBasicAttack(AEnemy* Target)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Target)
	{
		return;
	}
	if (bAttackLocked)
	{
		return;
	}
	
	// Checks Distance
	const float Dist = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());
	if (Dist > AttackRange)
	{
		return;
	}
	
	CurrentTarget = Target;
	// Starts first combo
	CurrentCombo = 1;
	MaxCombo = 1;
	OpenComboWindow();
	// Prevents attack before anim play ends
	bAttackLocked = true;
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(Timer_AttackLock, this, &UCombatComponent::UnlockAttack, AttackLockTime, false);
	}
	if (AHuman* H = Cast<AHuman>(Owner))
	{
		H->BP_PlayBasicAttack();
	}	
	OnCombatUIChanged.Broadcast();
}

void UCombatComponent::NotifyAttackHit()
{
	if (!CurrentTarget)
	{
		return;
	}
	// Deals Damage
	if (UHealthComponent* H = CurrentTarget->FindComponentByClass<UHealthComponent>())
	{
		H->ApplyDamage(BasicAttackDamage);
	}
}

float UCombatComponent::GetComboWindowPercent() const
{
	if (!bComboWindowOpen)
	{
		return 0.f;
	}
	
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float Remain = FMath::Max(0.f, ComboWindowEndTime - Now);
	return (ComboWindowDuration > 0.f) ? (Remain / ComboWindowDuration) : 0.f;
}

void UCombatComponent::UnlockAttack()
{
	bAttackLocked = false;
}

void UCombatComponent::CloseComboWindow()
{
	UE_LOG(LogTemp, Warning, TEXT("ComboWindow closes"));
	bComboWindowOpen = false;
	CurrentCombo = 0;
	OnCombatUIChanged.Broadcast();
}

void UCombatComponent::OpenComboWindow()
{
	bComboWindowOpen = true;
	UE_LOG(LogTemp, Warning, TEXT("ComboWindow opens"));
	
	if (UWorld* W = GetWorld())
	{
		ComboWindowEndTime = W->GetTimeSeconds() + ComboWindowDuration;
		W->GetTimerManager().ClearTimer(Timer_CloseWindow);
		W->GetTimerManager().SetTimer(Timer_CloseWindow, this, &UCombatComponent::CloseComboWindow, ComboWindowDuration, false);
	}
	OnCombatUIChanged.Broadcast();
}

