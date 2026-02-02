// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Component/SkillComponent.h"

#include "HealthComponent.h"
#include "Core/Char/Enemy.h"
#include "Core/Char/Human.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillComponent::TrySkill1(AEnemy* Target)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Target) return;
	
	// Prevents skill duplication
	if (bSkill1Active) return;
	// Checks distance
	const float Dist = FVector::Dist(Owner->GetActorLocation(), Target->GetActorLocation());
	if (Dist > Skill1Range) return;
	
	CurrentTarget = Target;
	bSkill1Active = true;
	
	// HumanBP plays anim
	if (AHuman* H = Cast<AHuman>(Owner))
	{
		H->BP_PlaySkill1();
	}
}

void USkillComponent::NotifySkill1Hit()
{
	if (!bSkill1Active || !CurrentTarget) return;
	
	if (UHealthComponent* Health = CurrentTarget->FindComponentByClass<UHealthComponent>())
	{
		Health->ApplyDamage(Skill1Damage);
	}
}

void USkillComponent::EndSkill1()
{
	bSkill1Active = false;
	CurrentTarget = nullptr;
}