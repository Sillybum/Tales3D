// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/CombatComponent.h"

#include "HealthComponent.h"
#include "Progression.h"
#include "GameFramework/Character.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::StartBasicAttack(AActor* TargetActor)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar || !BasicAttackMontage)
	{
		return;
	}
	
	CurrentTarget = TargetActor;
	CachedMaxCombo = GetMaxBasicComboFromLevel();
	
	// if combo timer just ended
	if (OwnerChar->GetWorld())
	{
		OwnerChar->GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
	}
	// if attacking, books next attack
	if (bIsAttacking)
	{
		if (ComboStep < CachedMaxCombo)
		{
			bQueueNextAttack = true;
		}
		return;
	}
	// if Combo Step over Max Combo Step at current level
	if (ComboStep > CachedMaxCombo)
	{
		ComboStep = 1;
	}
	
	bIsAttacking = true;
	bQueueNextAttack = false;
	PlayComboStep();
}

void UCombatComponent::Notify_BasicAttackHit()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	AActor* Target = CurrentTarget.Get();
	if (!OwnerChar || !Target)
	{
		return;
	}
	
	// Checks Distance
	const float Dist = FVector::Dist(OwnerChar->GetActorLocation(), Target->GetActorLocation());
	if (Dist > AttackRange)
	{
		return;
	}
	// Apply Damage to target's Health Component
	if (UHealthComponent* HC = Target->FindComponentByClass<UHealthComponent>())
	{
		HC->ApplyDamage(DamagePerHit);
	}
}

int32 UCombatComponent::GetMaxBasicComboFromLevel() const
{
	// Level 6/11/16/21 : +1
	const AActor* Owner = GetOwner();
	const UProgression* Progression = Owner ? Owner->FindComponentByClass<UProgression>() : nullptr;
	const int32 Lv = Progression ? Progression->GetLevel() : 1;
	
	int32 MaxCombo = 1;
	if (Lv >= 6)	MaxCombo++;
	if (Lv >= 11)	MaxCombo++;
	if (Lv >= 16)	MaxCombo++;
	if (Lv >= 21)	MaxCombo++;
	
	return FMath::Clamp(MaxCombo, 1, 5);
}

void UCombatComponent::PlayComboStep()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar || !BasicAttackMontage)
	{
		return;
	}
	// Chooses Section Name
	const int32 Index = FMath::Clamp(ComboStep - 1, 0, ComboSections.Num() - 1);
	const FName Section = ComboSections.IsValidIndex(Index) ? ComboSections[Index] : NAME_None;
	
	// Plays Montage (from the chosen section)
	OwnerChar->PlayAnimMontage(BasicAttackMontage, 1.f, Section);
	// Sets callback for Exiting Montage
	if (USkeletalMeshComponent* Mesh = OwnerChar->GetMesh())
	{
		if (UAnimInstance* Anim = Mesh->GetAnimInstance())
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UCombatComponent::OnMontageEnded);
			Anim->Montage_SetEndDelegate(EndDelegate, BasicAttackMontage);
		}
	} 
}

void UCombatComponent::ResetCombo()
{
	ComboStep = 1;
}

void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != BasicAttackMontage)
	{
		return;
	}
	
	CachedMaxCombo = GetMaxBasicComboFromLevel();
	// plays if there's booked attack && Max combo not reached
	if (bQueueNextAttack && ComboStep < CachedMaxCombo)
	{
		bQueueNextAttack = false;
		ComboStep++;
		PlayComboStep();
		return;
	}
	
	// Exiting Attack
	bIsAttacking = false;
	bQueueNextAttack = false;
	
	// if basic-attacks within ComboGraceTime, prolongs Combo Step
	if (AActor* Owner = GetOwner())
	{
		Owner->GetWorldTimerManager().SetTimer(
			ComboResetTimer,
			this,
			&UCombatComponent::ResetCombo,
			ComboGraceTime,
			false
			);
	}
}