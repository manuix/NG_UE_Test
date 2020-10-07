// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "NG_Test_MO\NG_Test_MOGameMode.h"
//#include "NG_Test_MO\NG_Test_MOCharacter.h"
#include "MyPlayerController.h"


AMyGameState::AMyGameState() {
	SetReplicates(true);
}


void AMyGameState::SetNextTurn() {
	//A very lazy loop thorugh all the playerstates. I have no idea how this will behave if players start disconnecting and reconecting.
	CurrentTurn = PlayerArray[(PlayerArray.IndexOfByKey(CurrentTurn) + 1) % PlayerArray.Num()];
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, Pyramid);
	DOREPLIFETIME(AMyGameState, bGameOver);
	DOREPLIFETIME(AMyGameState, CurrentTurn);
}

void AMyGameState::OnRep_Pyramid() {
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyPlayerController::StaticClass(), actors);
	//When the pyramid gets replicated we notify the server. This keeps the pyramids consitent across all clients.
	if (actors.Num() > 0) {
		auto controller = (AMyPlayerController*)actors[0];
		if (controller)
			controller->Server_NotifyPyrSet();
	}
}

void AMyGameState::OnRep_CurrentTurn() {
	RepTurn.ExecuteIfBound(CurrentTurn);
}
void AMyGameState::OnRep_GameOver() {
	if (bGameOver) {
		//We wait some seconds so that all the clients get the score from the last move replicated.
		FTimerDelegate tDelegate;
		tDelegate.BindLambda([this] {
			RepGameOver.ExecuteIfBound(PlayerArray);
			});
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, tDelegate, 1.0f, false);
	}
}

void AMyGameState::Client_Log_Implementation(const FString& log) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Cyan, log);
	}
}