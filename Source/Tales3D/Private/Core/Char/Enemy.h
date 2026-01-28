// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UWidgetComponent;
class UHealthComponent;

UCLASS()
class AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy|UI")
	TObjectPtr<UWidgetComponent> HPBarWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy|UI")
	FVector HPBarOffset = FVector(0, 0, -220);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy|UI")
	FVector2D HPBarDrawSize = FVector2D(110, 11);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	bool bIsSelected = false;
	
public:
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void SetSelected(bool bSelected);
	UFUNCTION(BlueprintPure, Category="Enemy")
	bool IsSelected() const { return bIsSelected; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	TObjectPtr<UHealthComponent> Health;
};
