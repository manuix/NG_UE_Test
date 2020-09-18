// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "NG_Test_MOGameMode.generated.h"

UCLASS(minimalapi)
class ANG_Test_MOGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANG_Test_MOGameMode();

	bool bGameOver;



protected:
	class AMyGameState* MyGameState;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void GeneratePyramid();
	bool IsGameOver();
	

};



