// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum

#pragma once

#include "CoreMinimal.h"
#include "Core/Data/WeaponItemData.h"
#include "GameFramework/Character.h"
#include "Human.generated.h"
// #include "Core/Data/ItemData.h"

enum class EPlayableCharacter : uint8;
class UCameraComponent;
class USpringArmComponent;
// Custom Components
class UInventory;
class UProgression;
class UVital;
class UEquipment;

UCLASS()
class AHuman : public ACharacter
{
	GENERATED_BODY()

public:
	AHuman();

protected:
	virtual void BeginPlay() override;
public:
	// Toggle (Run/Walk)
	UFUNCTION(BlueprintCallable, Category="MoveMode")
	void ToggleRunMode();
	
	// 컷씬 등에서 사용할 토글(블루프린트에서 적용 가능하도록)
	UFUNCTION(BlueprintCallable, Category="MoveMode")
	void SetRunMode(bool bRun);
	// 애님블루프린트에서 읽을 값
	UFUNCTION(BlueprintPure, Category="MoveMode")	
	bool IsRunning() const { return bIsRunning; }
	// Topdown Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;
	
	// Speed (Walk/Run)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MoveMode")
	float WalkSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MoveMode")
	float RunSpeed = 600.f;
	
	// 애님블루프린트가 읽을 실제 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MoveMode")
	bool bIsRunning = false;
	
	// Custom Components
	// Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UInventory> Inventory;
	// Level & Status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Progression")
	TObjectPtr<UProgression> Progression;
	// HP & MP & SP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vital")
	TObjectPtr<UVital> Vital;
	// Equipment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Equipment")
	TObjectPtr<UEquipment> Equipment;
	// Character Type for Equipment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character")
	EPlayableCharacter CharacterType = EPlayableCharacter::Maximin;
private:
	void ApplyMoveSpeed();
};
