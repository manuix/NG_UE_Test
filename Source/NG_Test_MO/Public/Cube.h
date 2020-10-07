// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Pyramid.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cube.generated.h"

UCLASS()
class NG_TEST_MO_API ACube : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ACube();

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(Replicated)
	//The pyramid this cube belongs to
	class APyramid* Pyramid;
	
	UPROPERTY(ReplicatedUsing = OnRep_Position)
	// The position in the pyramid. See reference document.
	FIntVector PyramidPosition;
	//Is this cube alive?
	bool bAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(ReplicatedUsing = OnRep_Color)
	//Hardoced variable that identifies the cube color
	//0 = Red | 1 = Green | 2 = Blue | (-inf , 0) U (2, +inf) = ERROR
	uint8 Color;
	UFUNCTION()
	void OnRep_Color();
	UFUNCTION()
	void OnRep_Position();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//0 = Red | 1 = Green | 2 = Blue | (-inf , 0) U (2, +inf) = ERROR
	void SetColor(uint8 newColor);
	//0 = Red | 1 = Green | 2 = Blue | (-inf , 0) U (2, +inf) = ERROR
	FORCEINLINE uint8 GetColor() { return Color; }

	//Turns on or off the Highlight on this cube.
	void SetHighlight(bool on);
	//Should be usefull for generating the pyramid again, and not alocatting new cubes
	void Spawn();
	//To be called by the server when it's exploded.
	void ExplodeCube();

	UFUNCTION(NetMulticast, reliable)
	//Destroys the cube on the client
	void Client_ExplodeCube();

};
