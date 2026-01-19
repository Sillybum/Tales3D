// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Framework/CoreController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Core/Char/Human.h"

void ACoreController::BeginPlay()
{
	Super::BeginPlay();
	
	// 탑다운 마우스 커서
	bShowMouseCursor		= true;
	bEnableClickEvents		= true;
	bEnableMouseOverEvents	= true;
}

void ACoreController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// 이동 마우스 입력 연결
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ACoreController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ACoreController::OnSetDestinationReleased);
	InputComponent->BindAction("ToggleRun", IE_Pressed, this, &ACoreController::OnToggleRun);
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
