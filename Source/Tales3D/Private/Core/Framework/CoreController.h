// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CoreController.generated.h"

class UUserWidget;
/**
 * 
 */
UCLASS()
class ACoreController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;
	
	// 마우스 클릭 이동
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void UpdateMoveDestination();
	
	// Toggle (Run/Walk)
	void OnToggleRun();
	
	// (Test) Inventory
	void OnInvAddTest();
	void OnInvAddTest2();
	
	// (Test) Progression
	void OnLevelUpTest();
	void OnIncreaseStat_STAB();
	void OnIncreaseStat_HACK();
	void OnIncreaseStat_INT();
	void OnIncreaseStat_DEF();
	void OnIncreaseStat_MR();
	void OnIncreaseStat_DEX();
	void OnIncreaseStat_AGI();
	
protected:
	// UI
	// For assigning Widget on editor
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	// Created Instance
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;
	
	
private:
	// 마우스 이동 틱
	bool bMoveHeld = false;
	float MoveUpdateInterval = 0.05f;
	float TimeSinceLastMoveUpdate = 0.f;
	
	// 커서가 크게 움직였을 때만 재계산(경로 재계산 낭비 방지)
	FVector LastGoal = FVector::ZeroVector;
	bool bHasLastGoal = false;
	// 50cm 이상 움직였을 때만 갱신
	float MinGoalDelta = FMath::Square(50.f);
};
