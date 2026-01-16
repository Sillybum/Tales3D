// Title: TalesWeaver 3D clone game / License: GPLv3 / Github: Sillybum


#include "Core/Framework/CoreGameMode.h"

#include "CoreController.h"

ACoreGameMode::ACoreGameMode()
{
	PlayerControllerClass = ACoreController::StaticClass();
}
