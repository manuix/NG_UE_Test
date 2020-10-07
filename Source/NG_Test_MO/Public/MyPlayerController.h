// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"



#include "MyPlayerController.generated.h"

/**
 *
 */
UCLASS()
class NG_TEST_MO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	UFUNCTION(Server, reliable, WithValidation)
	//Tells the server that the client has allocated the pyramid
	void Server_NotifyPyrSet();

	//Not Needed. We handle this by attaching the ShowLeaderboard Func to the GameIsOver Delegate on the game state.
	//UFUNCTION(NetMulticast, reliable)
	//Called by the server to inform all the controllers that the game is over
	//void Multicast_GameIsOver();

protected:
	//Base class for the UI.
	TSubclassOf<class UUserWidget> BUI;
	//UI Instance
	class UUserWidget* MyUI;
	FString CurrentlyDisplayedTurn;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//Function we attach to different delegates on the playerState and the game state in order to update the UI.

	//Updates the Score on the UI
	void UpdateScore(int32 newScore);
	//Updates the Name on the UI
	void UpdateName(FString newName);
	//Updates the Current Turn on the UI
	void UpdateTurn(APlayerState* newTurn);

	//Shows the leaderboard with all the players and their scores, ordered by score. Should only be called whtn the game is over.
	void ShowLeaderboard(TArray<APlayerState*> pss);

	virtual void OnRep_PlayerState() override;

};
