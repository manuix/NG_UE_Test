// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/ListView.h"

#include "MyPlayerState.h"
#include "MyGameState.h"
#include "ScoreEntry.h"


AMyPlayerController::AMyPlayerController() {
	PrimaryActorTick.bCanEverTick = true;
	ConstructorHelpers::FClassFinder<UUserWidget> UIWidgetFind(TEXT("/Game/Mine/UI_Widget"));
	BUI = UIWidgetFind.Class;

	SetReplicates(true);
}

void AMyPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (!IsLocalPlayerController()) {
		return;
	}

	//Setting up the HUD
	MyUI = CreateWidget<UUserWidget>(this, BUI);
	if (MyUI) {
		MyUI->AddToViewport();
	}

	//Adding delegates to the replication of GameState vars, in order to update the UI.
	auto MyGameState = GetWorld()->GetGameState<AMyGameState>();
	if (MyGameState) {
		//We don't need to bind this if we are doing this on the Tick function already.
		//MyGameState->RepTurn.BindUObject(this, &AMyPlayerController::UpdateTurn);
		MyGameState->RepGameOver.BindUObject(this, &AMyPlayerController::ShowLeaderboard);
	}

}

void AMyPlayerController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	//This shouldn't be necessary, but just in case. Shouldn't be too much of a burden, right?
	if (!IsLocalPlayerController())
		return;

	//Updating the turn on the tick function because it was a bit hard to keep it consistent otherwise.
	auto gs = GetWorld()->GetGameState<AMyGameState>();
	if (gs) {
		if (gs->CurrentTurn != nullptr) {
			if (gs->CurrentTurn->GetPlayerName() != CurrentlyDisplayedTurn)
				UpdateTurn(gs->CurrentTurn);
		}
	}
}


//UI Delegates
void AMyPlayerController::UpdateScore(int32 newScore) {
	if (MyUI) {
		auto scoreWidget = MyUI->WidgetTree->FindWidget<UTextBlock>("Score_Val");
		if (scoreWidget)
			scoreWidget->SetText(FText::FromString(FString::FromInt(newScore)));
	}
}

void AMyPlayerController::UpdateName(FString newName) {
	if (MyUI) {
		auto nameWidget = MyUI->WidgetTree->FindWidget<UTextBlock>("Name");
		if (nameWidget)
			nameWidget->SetText(FText::FromString(newName));
	}

}


void AMyPlayerController::UpdateTurn(APlayerState* newTurn) {
	if (!MyUI)
		return;
	auto currentTurn = MyUI->WidgetTree->FindWidget<UTextBlock>("Turn_Val");
	if (currentTurn) {
		currentTurn->SetText(FText::FromString(newTurn->GetPlayerName()));
		CurrentlyDisplayedTurn = newTurn->GetPlayerName();
	}
}

void AMyPlayerController::ShowLeaderboard(TArray<APlayerState*> players) {
	if (MyUI) {
		auto leaderboardContainer = (UBorder*)MyUI->WidgetTree->FindWidget("ScoreLeaderboardContainer");
		if (leaderboardContainer) {
			leaderboardContainer->SetVisibility(ESlateVisibility::Visible);

			auto leaderboardList = (UListView*)MyUI->WidgetTree->FindWidget("ScoreList");
			if (leaderboardList) {
				TArray<AActor*> players;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerState::StaticClass(), players);
				TArray<UScoreEntry*> entries = TArray<UScoreEntry*>();
				for (int32 i = 0; i < players.Num(); i++)
				{
					auto player = Cast<APlayerState>(players[i]);

					auto entry = NewObject<UScoreEntry>(this);
					entry->Setup(i + 1, player->GetPlayerName(), player->GetScore());

					entries.Add(entry);
				}
				//Sorting entries by score
				entries.Sort([](const UScoreEntry& LHS, const UScoreEntry& RHS) {return LHS.PlayerScore > RHS.PlayerScore; });
				//Adding the entries to the leaderboard
				for (int32 i = 0; i < entries.Num(); i++) {
					entries[i]->PlayerPosition = i+1;
					leaderboardList->AddItem(entries[i]);
				}
			}
		}
	}
}

//End UI Delegates

bool AMyPlayerController::Server_NotifyPyrSet_Validate() {
	return true;
}
void AMyPlayerController::Server_NotifyPyrSet_Implementation() {
	auto state = GetWorld()->GetGameState<AMyGameState>();
	if (state != nullptr) {
		state->ClientWithPyramidCount++;
	}
	//This should only run in the server so it's a bit unnecesary to check if it's null i guess.
	auto gm = GetWorld()->GetAuthGameMode<ANG_Test_MOGameMode>();
	if (gm) {
		gm->CheckClientsWithPyramidCount();
	}
}


void AMyPlayerController::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	auto myPS = (AMyPlayerState*)PlayerState;
	if (!myPS) {
		return;
	}

	myPS->RepScore.BindUObject(this, &AMyPlayerController::UpdateScore);
	myPS->RepName.BindUObject(this, &AMyPlayerController::UpdateName);

	//Showing the PlayerState name on the UI when first replicated.
	UpdateName(myPS->GetPlayerName());

}