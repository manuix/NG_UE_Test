// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NG_Test_MOHUD.generated.h"

UCLASS()
class ANG_Test_MOHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UFont* font;

public:
	ANG_Test_MOHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

