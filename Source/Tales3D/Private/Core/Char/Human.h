// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Human.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class AHuman : public ACharacter
{
	GENERATED_BODY()

public:
	AHuman();

protected:
	virtual void BeginPlay() override;
	
	// Topdown Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;
};
