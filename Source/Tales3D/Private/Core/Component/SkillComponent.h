// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"


class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillComponent();
	
	// Simple Skill Implementation
	UFUNCTION(BlueprintCallable, Category="Skill")
	void TrySkill1(AEnemy* Target);
	// Processes Damage, called from AnimNotify
	UFUNCTION(BlueprintCallable, Category="Skill")
	void NotifySkill1Hit();
	// Ends skill, called at the end of montage
	UFUNCTION(BlueprintCallable, Category="Skill")
	void EndSkill1();
	UFUNCTION(BlueprintPure, Category="Skill")
	bool IsSkill1Active() const { return bSkill1Active; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	float Skill1Range = 250.f;
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	int32 Skill1Damage = 25;
	
private:
	UPROPERTY()
	TObjectPtr<AEnemy> CurrentTarget;
	
	bool bSkill1Active = false;
};
