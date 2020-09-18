// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

#include "NG_Test_MOGameMode.h"

ANG_Test_MOHUD::ANG_Test_MOHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;


}

void ANG_Test_MOHUD::BeginPlay() {
	Super::BeginPlay();

	GameMode = GetWorld()->GetAuthGameMode<ANG_Test_MOGameMode>();
}

void ANG_Test_MOHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
											(Center.Y + 0.0f));
	//(Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	float size = 3;
	DrawText(GameMode->bGameOver() ? "Game Over!" : "", FLinearColor::Red, Center.X - (25 * size), Center.Y - (25 * size), nullptr, size, false);

}
