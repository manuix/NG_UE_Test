// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

#include "NG_Test_MOCharacter.h"
#include "NG_Test_MOGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ANG_Test_MOHUD::ANG_Test_MOHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;


}

void ANG_Test_MOHUD::BeginPlay() {
	Super::BeginPlay();


	GameState = (AGameState*)UGameplayStatics::GetGameState(GetWorld());
	if (GetWorld()->GetFirstPlayerController<APlayerController>()) {
		player = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
	}
	else if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "No player controller on world?");
	}
}

void ANG_Test_MOHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y + 0.0f));
	//(Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	float GameOverSize = 3;
	//TODO
	//Notice GameOver!
	//DrawText(GameMode->bGameOver() ? "Game Over!" : "", FLinearColor::Red, Center.X - (25 * GameOverSize), Center.Y - (25 * GameOverSize), nullptr, GameOverSize, false);


	//DrawText(GetWorld->)
	float scoreSize = 2;
	if (player != nullptr)
		DrawText(FString::Printf(TEXT("Score: %i"), (uint32)player->GetScore()), FLinearColor::White, (25 * scoreSize), (25 * scoreSize), nullptr, scoreSize, false);
	else {
		DrawText("No player to show score of.", FLinearColor::White, (25 * scoreSize), (25 * scoreSize), nullptr, scoreSize, false);
	}

}
