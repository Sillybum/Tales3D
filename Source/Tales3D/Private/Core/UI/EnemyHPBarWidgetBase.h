// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPBarWidgetBase.generated.h"

class UHealthComponent;

UCLASS()
class UEnemyHPBarWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="EnemyHP")
	TObjectPtr<AActor> TargetActor;
	UPROPERTY(BlueprintReadOnly, Category="EnemyHP")
	TObjectPtr<UHealthComponent> HealthComponent;
	UFUNCTION()
	void HandleHealthChanged(int32 Current, int32 Max);
	
public:
	UFUNCTION(BlueprintCallable, Category="EnemyHP")
	void SetTargetActor(AActor* InActor);
	UFUNCTION(BlueprintPure, Category="EnemyHP")
	float GetHPPercent() const;
	UFUNCTION(BlueprintPure, Category="EnemyHP")
	FText GetHPText() const;
	
	// Provides BP the timing to refresh value
	UFUNCTION(BlueprintImplementableEvent, Category="EnemyHP")
	void BP_OnTargetSet();
	UFUNCTION(BlueprintImplementableEvent, Category="EnemyHP")
	void BP_OnHealthChanged(int32 Current, int32 Max);
};
