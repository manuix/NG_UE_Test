// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class NG_TEST_MO_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	class APyramid* Pyramid;
	bool bGameOver;

};
