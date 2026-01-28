// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/UI/EnemyHPBarWidgetBase.h"

#include "Core/Component/HealthComponent.h"

void UEnemyHPBarWidgetBase::HandleHealthChanged(int32 Current, int32 Max)
{
	BP_OnHealthChanged(Current, Max);
}

void UEnemyHPBarWidgetBase::SetTargetActor(AActor* InActor)
{
	TargetActor = InActor;
	HealthComponent = nullptr;
	
	if (TargetActor)
	{
		HealthComponent = TargetActor->FindComponentByClass<UHealthComponent>();
		if (HealthComponent)
		{
			HealthComponent->OnHealthChanged.AddDynamic(this, &UEnemyHPBarWidgetBase::HandleHealthChanged);
		}
	}
	
	BP_OnTargetSet();
	
	// Refreshes at first
	if (HealthComponent)
	{
		HandleHealthChanged(HealthComponent->GetCurrent(), HealthComponent->GetMax());
	}
}

float UEnemyHPBarWidgetBase::GetHPPercent() const
{
	return HealthComponent ? HealthComponent->GetCurrent() : 0.f;
}

FText UEnemyHPBarWidgetBase::GetHPText() const
{
	if (!HealthComponent)
	{
		return FText::FromString(TEXT("0/0"));
	}
	return  FText::FromString(FString::Printf(TEXT("%d / %d"), HealthComponent->GetCurrent(), HealthComponent->GetMax()));
}
