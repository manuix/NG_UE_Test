// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NG_TEST_MO_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyPlayerState();

	DECLARE_DELEGATE_OneParam(int32Delegate, int32)
	//Delegate invoked when the score is replicated from the server
	int32Delegate RepScore;

	DECLARE_DELEGATE_OneParam(FStringDelegate, FString)
	//Delegate invoked when the name is replicated from the server
	FStringDelegate RepName;

protected: 
	virtual void OnRep_Score() override;
	virtual void OnRep_PlayerName() override;
	
};
