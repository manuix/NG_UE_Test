// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "NG_Test_MO/NG_Test_MOGameMode.h"

#include "MyGameState.generated.h"

UCLASS()
class NG_TEST_MO_API AMyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AMyGameState();

	UPROPERTY(ReplicatedUsing = OnRep_Pyramid)
	//Currently used Pyramid reference.
	class APyramid* Pyramid;
	UPROPERTY(ReplicatedUsing = OnRep_GameOver)
	//Is the game over? Defined by the gamemode.
	bool bGameOver;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTurn)
	//The PlayerState that should make the next move
	class APlayerState* CurrentTurn;

	//This is only usefull on the server. It counts the number of clients that have the pyramid set.
	uint8 ClientWithPyramidCount;
	TSet<class AMyPlayerController*> controllersWithNameSet;

	UFUNCTION(NetMulticast, reliable)
	//Debug function used to print messages on clients from the auth-server.
	void Client_Log(const FString& log);


	//Checks the player array for the next player to get a turn.
	void SetNextTurn();



	UFUNCTION()
	//Executed when the pyramid is set. This will notify the server we are ready for it to generate the pyramid.
	void OnRep_Pyramid();

	DECLARE_DELEGATE_OneParam(APlayerStatePtrDelegate, class APlayerState*)
	UFUNCTION()
	void OnRep_CurrentTurn();
	//Executed when the turn changes
	APlayerStatePtrDelegate RepTurn;

	DECLARE_DELEGATE_OneParam(TArray_APlayerStatePtrDelegate, TArray<class APlayerState*>)
	UFUNCTION()
	void OnRep_GameOver();
	//Executed when the game is over
	TArray_APlayerStatePtrDelegate RepGameOver;


};
