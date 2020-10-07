// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOGameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"

#include "NG_Test_MOHUD.h"
#include "NG_Test_MOCharacter.h"
#include "NG_Test_MOCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "Cube.h"
#include "Pyramid.h"
#include "MyGameState.h"


ANG_Test_MOGameMode::ANG_Test_MOGameMode()
	: Super()
{

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use all my custom classes
	HUDClass = ANG_Test_MOHUD::StaticClass();
	GameStateClass = AMyGameState::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();

	bDelayedStart = true;

}

void ANG_Test_MOGameMode::BeginPlay() {
	Super::BeginPlay();

	MyGameState = (AMyGameState*)GameState;

}

AActor* ANG_Test_MOGameMode::ChoosePlayerStart_Implementation(AController* player) {
	Super::ChoosePlayerStart_Implementation(player);
	TArray<AActor*> starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), starts);
	//Loops through all the starts and sets the player start. I can only hope the above Get always returns the starts in the same order.
	//I could save the array and save the execution and be sure about it, but i'm not too worried.
	if (starts.Num() > 0) {
		return starts[GetNumPlayers() % starts.Num()];
	}
	else {
		return nullptr;
	}
}

bool ANG_Test_MOGameMode::ReadyToStartMatch_Implementation() {

	if (GetNumPlayers() > 1) {
		return true;
	}
	return false;
}

void ANG_Test_MOGameMode::HandleMatchHasStarted() {
	Super::HandleMatchHasStarted();

	//We Create and set the pyramid object.
	//Then we have to wait for the clients to have that set up, in order to let the cubes set up on the clients correctly.
	if (MyGameState->Pyramid == nullptr) {
		MyGameState->Pyramid = GetWorld()->SpawnActor<APyramid>();
		MyGameState->Pyramid->SetActorLocation(FVector(310.000000, -540.000000, 220.000000));
		MyGameState->Pyramid->SetOwner(GetGameState<AMyGameState>());
	}

	//Setting random names
	for (auto player : MyGameState->PlayerArray)
	{
		auto i = FMath::RandRange(0, Names.Num() - 1);
		player->SetPlayerName(Names[i]);
		Names.RemoveAt(i);
	}

	//rng first turn
	MyGameState->CurrentTurn = MyGameState->PlayerArray[FMath::RandRange(0, MyGameState->PlayerArray.Num() - 1)];

}

void ANG_Test_MOGameMode::CheckClientsWithPyramidCount() {
	if (MyGameState && MyGameState->ClientWithPyramidCount >= NumPlayers - 1) {
		GeneratePyramid();
	}
}

void ANG_Test_MOGameMode::GeneratePyramid() {
	if (MyGameState == nullptr) {
		return;
	}

	MyGameState->Pyramid->Server_GeneratePyramid();

}

bool ANG_Test_MOGameMode::ReadyToEndMatch_Implementation() {
	if (!MyGameState || !MyGameState->Pyramid || MyGameState->GetGameTimeSinceCreation() < 5)
		return false;

	if (!MyGameState->Pyramid->IsAnyCubeAlive()) {
		//Setting this as true, to let clients know.
		MyGameState->bGameOver = true;
		return true;
	}
	return false;
}


bool ANG_Test_MOGameMode::IsGameOver() {
	auto pyr = MyGameState->Pyramid;
	if (pyr == nullptr)
		return true;
	if (!pyr->IsAnyCubeAlive()) {
		return true;
	}

	return false;

}