// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class NG_TEST_MO_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	//UPROPERTY(Replicated)
	class APyramid* Pyramid;

	uint8 turn;

};
