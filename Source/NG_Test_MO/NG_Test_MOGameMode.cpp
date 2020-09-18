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
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use my custom HUD class
	HUDClass = ANG_Test_MOHUD::StaticClass();
	// use my custom GameState class
	GameStateClass = AMyGameState::StaticClass();

}

void ANG_Test_MOGameMode::BeginPlay() {
	Super::BeginPlay();

	GeneratePyramid();

	MyGameState = (AMyGameState*)GameState;
	//Aca le agregaria un callback a Pyramid->ExplodeCube(), para checkear si quedan cubos o si termino el juego.
	//Sin embargo, no se como haria eso en c++. Onda seria un delegate, pero anda a saber.
	//Lo hacemos en el tick y ya fue.
	//GetGameState<AMyGameState>()->Pyramid->;
}

void ANG_Test_MOGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(1234, 1, FColor::White, "Hi im the game mode ticking");

	MyGameState->bGameOver = IsGameOver();
}

void ANG_Test_MOGameMode::GeneratePyramid() {
	auto gameState = GetGameState<AMyGameState>();
	if (gameState->Pyramid == nullptr) {
		gameState->Pyramid = GetWorld()->SpawnActor<APyramid>();
		gameState->Pyramid->SetActorLocation(FVector(310.000000, -540.000000, 220.000000));
		gameState->Pyramid->SetOwner(GetGameState<AMyGameState>());
	}
	gameState->Pyramid->GeneratePyramid();
}

bool ANG_Test_MOGameMode::IsGameOver() {
	auto pyr = MyGameState->Pyramid;
	if (pyr == nullptr)
		return true;

	return !pyr->IsAnyCubeAlive();

}
