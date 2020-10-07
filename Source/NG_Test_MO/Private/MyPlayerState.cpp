// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

AMyPlayerState::AMyPlayerState() {
	SetReplicates(true);
}

void AMyPlayerState::OnRep_Score() {
	Super::OnRep_Score();
	RepScore.ExecuteIfBound(GetScore());
}

void AMyPlayerState::OnRep_PlayerName() {
	Super::OnRep_PlayerName();

	if (RepName.IsBound()) {
		RepName.ExecuteIfBound(GetPlayerName());
	}
}
