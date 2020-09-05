// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOGameMode.h"
#include "NG_Test_MOHUD.h"
#include "NG_Test_MOCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANG_Test_MOGameMode::ANG_Test_MOGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANG_Test_MOHUD::StaticClass();
}
