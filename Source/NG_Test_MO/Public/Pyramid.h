// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Cube.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pyramid.generated.h"

UCLASS()
class NG_TEST_MO_API APyramid : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	class USceneComponent* RootSceneComponent;

public:
	// Sets default values for this actor's properties
	APyramid();

	//Base class for the vube to be spawned.
	TSubclassOf<class ACube> CubeClass;

	UPROPERTY(EditAnywhere, Category = "PyramidSettings")
	uint8 Height;

	FORCEINLINE bool IsAnyCubeAlive() { return bAnyCubeAlive; };

	//Returns the 6 neighboring cubes. Might return null values.
	TArray<ACube*> GetTouchingCubes(uint8 x, uint8 y);
	//Returns the 6 neighboring cubes. Might return null values.
	TArray<ACube*> GetTouchingCubes(ACube* cube);

	//Returns all the same colored cubes. Returns an empty array if invalid cube coordiantes are passed.
	TSet<ACube*> GetGroupCubes(uint8 x, uint8 y);
	//Returns all the same colored cubes. Returns an empty array if invalid cube.
	TSet<ACube*> GetGroupCubes(ACube* cube);

	//Given a set of coordinates, will return the position in the CubesArray for said cube. Can return -1 if out of range. Check first with IsCoordinateWithinRange.
	uint16 GetPositionInArray(uint8 x, uint8 y);

	//Returns the cube with the given coordinates. Can return null.
	ACube* GetCubeAt(uint8 x, uint8 y);

	//Sets the cube at the given coordinates. Also will set the cube's internal coordinates. Won't do anything if the coordinates are out of range.
	//Note: If the cube is already in the CubeArray, it will move it, and set the previous position in the array to null.
	void SetCubeAt(uint8 x, uint8 y, ACube* cube);

	UFUNCTION(Server, reliable, WithValidation)
	//Called by the server to generate the pyramid.
	void Server_GeneratePyramid();


	//Explodes the cube at the given coordiantes
	uint32 ExplodeCube(uint8 x, uint8  y);
	//Explodes the given cube, along with all the same colored touching cubes, returning the amount of points to award
	uint32 ExplodeCube(ACube* cube);

	UFUNCTION(NetMulticast, reliable)
	//Called on the client after the cube is exploded on the server.
	void Client_ExplodeCube(ACube* Cube);

	//Highlights the group of cubes of the same color as the given cube.
	void SetHighlightGroup(ACube* cube, bool on);

protected:
	//Generated cubes are stored here. See ref file for more info on how they are placed.
	TArray<ACube*> CubesArray;
	//Updated after any cube is exploded. Used to check if the game is over.
	bool bAnyCubeAlive;

	//Are the given coordinates within the cubes array
	bool IsCoordinateWithinRange(int16 x, int16 y);

	UFUNCTION(NetMulticast, reliable)
	//Drops the cube vertically in the array.
	//To be called after exploding any cube on the server. Done on the server and clients.
	void Multicast_RefreshCubesPositions();

	//Non-Recursive Fibonacci sequence. Probably declared in a not so great spot.
	uint32 GoodFib(uint16 x);

	//I'm unsure why this is here. Probably here for the server to wait for the clients.
	bool bCubesArrayInit;
	UFUNCTION(NetMulticast, reliable, WithValidation)
	//I'm unsure why this is here.
	void Multicast_InitArray();

};
