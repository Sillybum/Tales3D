// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Char/Enemy.h"

#include "Components/WidgetComponent.h"
#include "Core/Component/HealthComponent.h"
#include "Core/UI/EnemyHPBarWidgetBase.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	
	HPBarWidget->SetupAttachment(RootComponent);
	
	// To face screen
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidget->SetDrawSize(HPBarDrawSize);
	HPBarWidget->SetVisibility(false);
	HPBarWidget->SetRelativeLocation(HPBarOffset);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Injects created widget who I am
	if (UUserWidget* W = HPBarWidget->GetUserWidgetObject())
	{
		if (UEnemyHPBarWidgetBase* HPW = Cast<UEnemyHPBarWidgetBase>(W))
		{
			HPW->SetTargetActor(this);
		}
	}
}

void AEnemy::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	if (HPBarWidget)
	{
		HPBarWidget->SetVisibility(bSelected);
	}
}