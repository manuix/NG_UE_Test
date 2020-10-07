// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ScoreEntry.generated.h"

/**
 * UObject to be placed on the leaderboard list when the game is over
 */
UCLASS()
class NG_TEST_MO_API UScoreEntry : public UObject
{
	GENERATED_BODY()

public:
	int32 PlayerPosition;
	FString PlayerName;
	int32 PlayerScore;
	void Setup(int32 position, FString name, int32 score);
};
