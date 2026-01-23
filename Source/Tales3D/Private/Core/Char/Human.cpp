// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Char/Human.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
// Custom
#include "Core/Component/Inventory.h"
#include "Core/Component/Progression.h"
#include "Core/Component/Vital.h"

AHuman::AHuman()
{
	PrimaryActorTick.bCanEverTick = false;

	// 컨트롤러 회전과 캐릭터 회전 분리
	bUseControllerRotationPitch	= false;
	bUseControllerRotationRoll	= false;
	bUseControllerRotationYaw	= false;
	
	// 이동방향으로 캐릭터 회전
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement	= true;
		MoveComp->RotationRate				= FRotator(0.f, 720.f, 0.f);
	}
	
	bIsRunning = false;
	ApplyMoveSpeed();
	
	// 카메라 붐 생성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	// 카메라 붐 거리/각도
	CameraBoom->TargetArmLength = 900.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	// 벽에 부딪혔을 때 줌인 방지
	CameraBoom->bDoCollisionTest = false;
	// 컨트롤러 회전 무시
	CameraBoom->bUsePawnControlRotation = false;
	// 월드 회전 고정
	CameraBoom->SetUsingAbsoluteRotation(true);
	// 카메라 생성
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;
	// Custom Components
	// Inventory
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
	// Level & Status
	Progression = CreateDefaultSubobject<UProgression>(TEXT("Progression"));
	// HP & MP & SP
	Vital = CreateDefaultSubobject<UVital>(TEXT("Vital"));
}

void AHuman::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHuman::ApplyMoveSpeed()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
	}
}

void AHuman::ToggleRunMode()
{
	bIsRunning = !bIsRunning;
	ApplyMoveSpeed();
}

void AHuman::SetRunMode(bool bRun)
{
	bIsRunning = bRun;
	ApplyMoveSpeed();
}
