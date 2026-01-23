// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Framework/CoreController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
// Custom
#include "Blueprint/UserWidget.h"
#include "Core/Char/Human.h"
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
	InputComponent->BindAction("InvPrint", IE_Pressed, this, &ACoreController::OnInvPrint);
	// (Test) Progression
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ACoreController::OnLevelUpTest);

	InputComponent->BindKey(EKeys::One,   IE_Pressed, this, &ACoreController::OnIncreaseStat_STAP);
	InputComponent->BindKey(EKeys::Two,   IE_Pressed, this, &ACoreController::OnIncreaseStat_HACK);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ACoreController::OnIncreaseStat_INT);
	InputComponent->BindKey(EKeys::Four,  IE_Pressed, this, &ACoreController::OnIncreaseStat_DEF);
	InputComponent->BindKey(EKeys::Five,  IE_Pressed, this, &ACoreController::OnIncreaseStat_MR);
	InputComponent->BindKey(EKeys::Six,   IE_Pressed, this, &ACoreController::OnIncreaseStat_DEX);
	InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &ACoreController::OnIncreaseStat_AGI);
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

void ACoreController::OnInvPrint()
{
	if (!GEngine)
	{
		return;
	}
	if (AHuman* H = Cast<AHuman>(GetPawn()))
	{
		if (!H->Inventory)
		{
			return;
		}
		for (const FInventoryItem& Item : H->Inventory->GetItems())
		{
			const int32 Qty = Item.Quantity;
			const FPrimaryAssetId Id = Item.PrimaryId;
			// (debug) Loading starts..
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan,
				FString::Printf(TEXT("Loading ItemData... (%s) x%d"),*Id.ToString(), Qty));
			// loads ItemData async
			H->Inventory->RequestItemDataAsync(Id, [this, H, Id, Qty](UItemData* Data)
			{
				if (!GEngine)
				{
					return;
				}
				if (!Data)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
						FString::Printf(TEXT("Failed to load ItemData: %s"), *Id.ToString()));
					return;
				}
				
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
					FString::Printf(TEXT("Loaded: %s x%d\n%s"),
						*Data->DisplayName.ToString(), Qty, *Data->Description.ToString()));
				
				// loads icon async
				if (H && H->Inventory)
				{
					H->Inventory->RequestIconAsync(Data->Icon, [Id](UTexture2D* Tex)
					{
						if (GEngine)
						{
							const FString TexName = Tex ? Tex->GetName() : TEXT("(null)");
							GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
								FString::Printf(TEXT("Icon loaded for %s: %s"), *Id.ToString(), *TexName));
						}
					});
				}
			});
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
		PrintProgression();
	}
}

void ACoreController::OnIncreaseStat_STAP()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::STAP);
		PrintProgression();
	}
}void ACoreController::OnIncreaseStat_HACK()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::HACK);
		PrintProgression();
	}
}
void ACoreController::OnIncreaseStat_INT()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::INT);
		PrintProgression();
	}
}void ACoreController::OnIncreaseStat_DEF()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::DEF);
		PrintProgression();
	}
}void ACoreController::OnIncreaseStat_MR()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::MR);
		PrintProgression();
	}
}void ACoreController::OnIncreaseStat_DEX()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::DEX);
		PrintProgression();
	}
}
void ACoreController::OnIncreaseStat_AGI()
{
	if (auto* P = GetProgression(this))
	{
		P->TryIncreaseStatus(EStatusType::AGI);
		PrintProgression();
	}
}

void ACoreController::PrintProgression()
{
	if (!GEngine)
	{
		return;
	}
	
	if (UProgression* P = GetProgression(this))
	{
		FString S = FString::Printf(TEXT("Level: %d | Unspent: %d | +%d/Level\n"),
			P->GetLevel(), P->GetUnspentPoints(), P->GetPointsPerLevelUp());
		
		const TArray<FStatusViewData> Status = P->GetAllStatusViewData();
		for (const FStatusViewData& V : Status)
		{
			S += FString::Printf(TEXT("- %s: %d (Cost: %d)\n"),
				*V.DisplayName.ToString(), V.Value, V.CostPerIncrease);
		}
		if (AHuman* H = Cast<AHuman>(GetPawn()))
		{
			if (H->Vital)
			{
				const auto List = H->Vital->GetAllResourceViewData();
				S += TEXT("\n[Vitals]\n");
				for (const FResourceViewData& R : List)
				{
					S += FString::Printf(TEXT("- %s: %d / %d (+%d Max/Level)\n"),
						*R.DisplayName.ToString(), R.Current, R.Max, R.MaxIncreasePerLevel);
				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, S);
	}
	// HP & MP & SP

	
}
