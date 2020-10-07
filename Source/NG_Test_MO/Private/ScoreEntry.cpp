// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreEntry.h"

void UScoreEntry::Setup(int32 position, FString name, int32 score) {
	PlayerPosition = position;
	PlayerName = name;
	PlayerScore = score;
}