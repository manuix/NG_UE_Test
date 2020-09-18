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
#include "MyGameState.h"


ANG_Test_MOGameMode::ANG_Test_MOGameMode()
	: Super()
{
	//Super();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANG_Test_MOHUD::StaticClass();
	// use my custom GameState class
	GameStateClass = AMyGameState::StaticClass();

	ConstructorHelpers::FClassFinder<APyramid> BPPyramidClassFinder(TEXT("/Game/Mine/MyBPClasses/BP_MyPyramid"));
	BPPyramid = BPPyramidClassFinder.Class;
}

void ANG_Test_MOGameMode::BeginPlay() {
	Super::BeginPlay();

	//Find Pyramid and store it.
	//TArray<AActor*> pyramids;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APyramid::StaticClass(), pyramids);
	//Pyramid = (APyramid*)pyramids[0];

	GeneratePyramid();

	//GetGameState<AGameStateBase>()->player;

}

void ANG_Test_MOGameMode::GeneratePyramid() {
	APyramid* pyramid = GetGameState<AMyGameState>()->Pyramid;
	if (pyramid == nullptr) {
		pyramid = GetWorld()->SpawnActor<APyramid>(BPPyramid);
		pyramid->SetActorLocation(FVector(310.000000, -540.000000, 220.000000));
		pyramid->SetOwner(GetGameState<AMyGameState>());
	}
	pyramid->Server_GeneratePyramid();
}



bool ANG_Test_MOGameMode::bGameOver() {
	auto pyr = GetGameState<AMyGameState>()->Pyramid;
	if (pyr == nullptr)
		return true;

	return !pyr->bAnyCubeAlive();

}


int ANG_Test_MOGameMode::AskExplodeCube(ANG_Test_MOCharacter* player, ACube* cube) {
	//player
	return 0;
	//return Pyramid->ExplodeCube(cube);
}