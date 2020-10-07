// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "NG_Test_MOGameMode.generated.h"

UCLASS(minimalapi)
class ANG_Test_MOGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANG_Test_MOGameMode();

	bool bGameOver;

	//To be called when any client notifies the server that they set up the pyramid on their gamestate.
	void CheckClientsWithPyramidCount();
	void CheckClientsWithNameSet(class AMyPlayerController* controllerWithNewName);
	//Is the game over. Checked after destroying a cube.
	bool IsGameOver();


protected:
	//Saving casted reference
	class AMyGameState* MyGameState;

	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	//Generates the pyramid!
	void GeneratePyramid();

	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void HandleMatchHasStarted() override;
	virtual bool ReadyToEndMatch_Implementation() override;

	//Some names
	TArray<FString> Names{ "Carlos", "Juan", "Manuel", "Nicolas" };

};

