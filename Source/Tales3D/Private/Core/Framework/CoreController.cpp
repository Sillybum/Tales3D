// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Framework/CoreController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
// Custom
#include "Blueprint/UserWidget.h"
#include "Core/Char/Enemy.h"
#include "Core/Char/Human.h"
#include "Core/Component/CombatComponent.h"
#include "Core/Component/Equipment.h"
#include "Core/Component/Inventory.h"
#include "Core/Component/Progression.h"
#include "Core/Component/Vital.h"
#include "Core/Data/ItemData.h"

void ACoreController::BeginPlay()
{
	Super::BeginPlay();
	
	// 탑다운 마우스 커서
	bShowMouseCursor		= true;
	bEnableClickEvents		= true;
	bEnableMouseOverEvents	= true;
	
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void ACoreController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// 이동 마우스 입력 연결
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ACoreController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ACoreController::OnSetDestinationReleased);
	InputComponent->BindAction("ToggleRun", IE_Pressed, this, &ACoreController::OnToggleRun);
	
	// (test) Inventory
	InputComponent->BindAction("InvAddTest", IE_Pressed, this, &ACoreController::OnInvAddTest);
	InputComponent->BindAction("InvAddTest2", IE_Pressed, this, &ACoreController::OnInvAddTest2);
	

	// (Test) Progression
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ACoreController::OnLevelUpTest);

	InputComponent->BindKey(EKeys::One,   IE_Pressed, this, &ACoreController::OnIncreaseStat_STAB);
	InputComponent->BindKey(EKeys::Two,   IE_Pressed, this, &ACoreController::OnIncreaseStat_HACK);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ACoreController::OnIncreaseStat_INT);
	InputComponent->BindKey(EKeys::Four,  IE_Pressed, this, &ACoreController::OnIncreaseStat_DEF);
	InputComponent->BindKey(EKeys::Five,  IE_Pressed, this, &ACoreController::OnIncreaseStat_MR);
	InputComponent->BindKey(EKeys::Six,   IE_Pressed, this, &ACoreController::OnIncreaseStat_DEX);
	InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &ACoreController::OnIncreaseStat_AGI);
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ACoreController::OnEquipSteelShadeTest);
	InputComponent->BindKey(EKeys::F, IE_Pressed, this, &ACoreController::OnBasicAttackPressed);
}

void ACoreController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (!bMoveHeld)
	{
		return;
	}
	
	TimeSinceLastMoveUpdate += DeltaTime;
	
	if (TimeSinceLastMoveUpdate >= MoveUpdateInterval)
	{
		TimeSinceLastMoveUpdate = 0.f;
		UpdateMoveDestination();
	}
}

// 커서 아래로 레이캐스팅
void ACoreController::OnSetDestinationPressed()
{
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		Hit
		);
	
	// 1. Case: Enemy Clicked
	if (bHit)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(Hit.GetActor()))
		{
			SelectEnemy(Enemy);
			// Stops move input
			bMoveHeld = false;
			StopMovement();
			return;
		}
	}
	// 2. Case: Other than enemy Clicked
	SelectEnemy(nullptr);
	bMoveHeld = true;
	// 누르는 순간 한 번 이동 시작
	UpdateMoveDestination();
}

void ACoreController::OnSetDestinationReleased()
{
	bMoveHeld = false;
	bHasLastGoal = false;
}

void ACoreController::UpdateMoveDestination()
{
	FHitResult Hit;
	
	const bool bHit = GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true,
		Hit
		);
	
	if (!bHit)
	{
		return;
	}
	
	const FVector Goal = Hit.Location;
	
	if (bHasLastGoal && FVector::DistSquared(Goal, LastGoal) < MinGoalDelta)
	{
		return;
	}
	
	LastGoal = Goal;
	bHasLastGoal = true;
	
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Goal);
}

void ACoreController::OnToggleRun()
{
	if (AHuman* Human = Cast<AHuman>(GetPawn()))
	{
		Human->ToggleRunMode();
	}
}

// (test) Inventory
void ACoreController::OnInvAddTest()
{
	if (AHuman* H = Cast<AHuman>(GetPawn()))
	{
		if (H->Inventory)
		{
			H->Inventory->AddItemByName(TEXT("Potion"), 1);
		}
	}
}

void ACoreController::OnInvAddTest2()
{
	if (AHuman* H = Cast<AHuman>(GetPawn()))
	{
		if (H->Inventory)
		{
			H->Inventory->AddItemByName(TEXT("SteelShade"), 1);
		}
	}
}

// (Test) Progression
static UProgression* GetProgression(ACoreController* PC)
{
	if (AHuman* H = Cast<AHuman>(PC->GetPawn()))
	{
		return H->Progression;
	}
	return nullptr;
}

void ACoreController::OnLevelUpTest()
{
	if (UProgression* P = GetProgression(this))
	{
		P->LevelUp();
	}
}

// Debugging message function erased
void ACoreController::OnIncreaseStat_STAB()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::STAB);
	}
}void ACoreController::OnIncreaseStat_HACK()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::HACK);
	}
}
void ACoreController::OnIncreaseStat_INT()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::INT);
	}
}void ACoreController::OnIncreaseStat_DEF()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::DEF);
	}
}void ACoreController::OnIncreaseStat_MR()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::MR);
	}
}void ACoreController::OnIncreaseStat_DEX()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::DEX);
	}
}
void ACoreController::OnIncreaseStat_AGI()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::AGI);
	}
}

void ACoreController::OnEquipSteelShadeTest()
{
	AHuman* H = Cast<AHuman>(GetPawn());
	if (!H || !H->Equipment)
	{
		return;
	}
	const FPrimaryAssetId SteelShadeId(UItemData::AssetTypeName, FName(TEXT("SteelShade")));
	
	H->Equipment->EquipWeaponByPrimaryId(SteelShadeId);
}

void ACoreController::OnBasicAttackPressed()
{
	AHuman* H = Cast<AHuman>(GetPawn());
	if (!H || !H->Combat)
	{
		UE_LOG(LogTemp, Warning, TEXT("onBasicattack can't find"));
		return;
	}
	
	// if only an enemy selected
	if (SelectedEnemy)
	{
		H->Combat->TryBasicAttack(SelectedEnemy);
	}
}

void ACoreController::SelectEnemy(AEnemy* NewEnemy)
{
	if (SelectedEnemy == NewEnemy)
	{
		return;
	}
	if (SelectedEnemy)
	{
		SelectedEnemy->SetSelected(false);
	}
	
	SelectedEnemy = NewEnemy;
	
	if (SelectedEnemy)
	{
		SelectedEnemy->SetSelected(true);
	}
}
