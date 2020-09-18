// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOGameMode.h"
#include "NG_Test_MOHUD.h"
#include "NG_Test_MOCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "NG_Test_MOCharacter.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

ANG_Test_MOGameMode::ANG_Test_MOGameMode()
	: Super()
{
	//Super();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANG_Test_MOHUD::StaticClass();
}

void ANG_Test_MOGameMode::BeginPlay() {
	Super::BeginPlay();

	//Find Pyramid and store it.
	TArray<AActor*> pyramids;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APyramid::StaticClass(), pyramids);
	Pyramid = (APyramid*)pyramids[0];

	//GetGameState<AGameStateBase>()->player;


}

bool ANG_Test_MOGameMode::bGameOver() {
	if (Pyramid == nullptr)
		return true;

	return !Pyramid->bAnyCubeAlive();

}

int ANG_Test_MOGameMode::AskExplodeCube(ANG_Test_MOCharacter* player, ACube* cube) {
	//player
	return Pyramid->ExplodeCube(cube);
}