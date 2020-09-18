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

	bool bGameOver();
	int AskExplodeCube(class ANG_Test_MOCharacter* player, class ACube* cube);

protected:
	virtual void BeginPlay() override;
	class TSubclassOf<class APyramid> BPPyramid;

	void GeneratePyramid();

};



