// Fill out your copyright notice in the Description page of Project Settings.


#include "Pyramid.h"
#include "Cube.h"
#include "MyGameState.h"
#include "NG_Test_MO/NG_Test_MOGameMode.h"

#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APyramid::APyramid()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FClassFinder<ACube> BPColoredCube(TEXT("/Game/Mine/MyBPClasses/BP_ColoredCube"));
	CubeClass = BPColoredCube.Class;

	Height = 7;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	RootSceneComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(RootSceneComponent);

	SetReplicates(true);
}

bool APyramid::Multicast_InitArray_Validate() {
	return true;
}

void APyramid::Multicast_InitArray_Implementation() {
	if (bCubesArrayInit)
		return;

	//Initializing the Cubes array, for some reason I can't remember.
	CubesArray.Empty(Height * Height);
	CubesArray.SetNum(Height * Height, false);
	CubesArray.Init(nullptr, Height * Height);

	bCubesArrayInit = true;
}

bool APyramid::Server_GeneratePyramid_Validate() {
	return true;
}
void APyramid::Server_GeneratePyramid_Implementation() {
	bAnyCubeAlive = true;

	//I have no clue if this is a memory leak. Probably. Maybe if it was called more than once.
	for (uint16 i = 0; i < CubesArray.Num(); i++)
	{
		if (CubesArray[i] && CubesArray[i]->bAlive) {
			CubesArray[i]->ExplodeCube();
			CubesArray[i]->Client_ExplodeCube();
		}
		CubesArray[i] = nullptr;
	}

	Multicast_InitArray();

	for (uint8 y = 0; y < Height; y++)
	{
		for (uint8 x = 0; x < Height - y; x++)
		{
			uint16 PositionInArray = GetPositionInArray(x * 2, y);
			ACube* cube;

			//Actualy Spawining the cube
			cube = GetWorld()->SpawnActor<ACube>(CubeClass);
			if (cube != nullptr) {

				cube->SetOwner(this);
				cube->SetActorLabel(FString::Printf(TEXT("Cube %i | %i"), x * 2, y));
				//IDK why this doesn't attach the cube to the pyramid on the World Outliner. That would be neat.
				cube->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				cube->SetActorRelativeLocation(GetActorLocation() + FVector(0, 110 * x + 55 * y, 110 * y), false, nullptr, ETeleportType::ResetPhysics);
				cube->PyramidPosition = FIntVector(x * 2, y, 0);

				//This makes the cube only fall down physically.
				cube->Mesh->BodyInstance.bLockYTranslation = true;
				cube->Mesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);

				//0 => red | 1 => green | 2 => blue | Any other thing is not garanteed to be handeled correctly.
				//TODO: Change this so that it generates a similar amount of cubes for every color.
				//My only idea is to limit the amount of cubes to about a third of the total amount of cubes,
				//but I don't believe that would make it much fairer, so I won't take the time.
				cube->SetColor(FMath::RandRange(0, 02));

				cube->Pyramid = this;
				cube->Spawn();
			}

			CubesArray[PositionInArray] = cube;
		}
	}

}

uint16 APyramid::GetPositionInArray(uint8 x, uint8 y) {
	if (!IsCoordinateWithinRange(x, y)) {
		return -1;
	}

	//Check the reference for this to make sense.
	uint16 ret = 0;
	for (uint8 i = 0; i < y; i++)
	{
		ret += (Height - i) * 2 - 1;
	}
	ret += x;

	return ret;
}

ACube* APyramid::GetCubeAt(uint8 x, uint8 y) {
	if (!IsCoordinateWithinRange(x, y))
		return nullptr;

	return CubesArray[GetPositionInArray(x, y)];
}

void APyramid::SetCubeAt(uint8 x, uint8 y, ACube* cube) {
	if (!bCubesArrayInit)
		Multicast_InitArray();

	if (!CubesArray.IsValidIndex(GetPositionInArray(x, y))) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Setting out of index range");
		return;
	}

	if (IsCoordinateWithinRange(x, y)) {
		if (cube != nullptr) {
			//Check if the cube was already in the pyramid
			if (IsCoordinateWithinRange(cube->PyramidPosition.X, cube->PyramidPosition.Y)) {
				uint16 cubeCurrentPosition = GetPositionInArray(cube->PyramidPosition.X, cube->PyramidPosition.Y);
				if (CubesArray[cubeCurrentPosition] == cube)
					CubesArray[cubeCurrentPosition] = nullptr;
			}
			//Setting the cube internal position to the new one
			cube->PyramidPosition = FIntVector(x, y, 0);
		}
		CubesArray[GetPositionInArray(x, y)] = cube;
	}
}

bool APyramid::IsCoordinateWithinRange(int16 x, int16 y) {
	return
		y < Height&&
		y >= 0 &&
		x < (Height - y) * 2 - 1 &&
		x >= 0
		;
}


//										  N, NE,  E, SE,  S, SW,  W, NW
static const TArray<int8> XDirection = { -1,  0,  2,  2,  1,  0, -2, -2 };
//										  N, NE,  E, SE,  S, SW,  W, NW
static const TArray<int8> YDirection = { 1,  1,  0, -1, -1, -1,  0,  1 };
enum Direction { N = 0, NE, E, SE, S, SW, W, NW };


TArray<ACube*> APyramid::GetTouchingCubes(uint8 x, uint8 y) {
	TArray<ACube*>ret;
	ret.SetNum(8);

	//Check every direction for neighbors
	for (uint8 i = 0; i < 8; i++)
	{
		int16 NewX = (int16)x + XDirection[i];
		int16 NewY = (int16)y + YDirection[i];
		ret[i] = GetCubeAt(NewX, NewY);
	}
	return ret;
}

TArray<ACube*> APyramid::GetTouchingCubes(ACube* cube) {
	if (cube == nullptr)
		return TArray<ACube*>();
	return GetTouchingCubes(cube->PyramidPosition.X, cube->PyramidPosition.Y);
}

TSet<ACube*> APyramid::GetGroupCubes(uint8 x, uint8 y) {
	return GetGroupCubes(CubesArray[GetPositionInArray(x, y)]);
}

TSet<ACube*> APyramid::GetGroupCubes(ACube* cube) {

	TSet<ACube*> groupCubes;

	//If null cube then return empty array
	if (cube == nullptr)
		return groupCubes;

	TSet<ACube*> checkedCubes;
	TQueue<ACube*> cubesToCheck;

	//This cube is the starting point.
	groupCubes.Add(cube);
	cubesToCheck.Enqueue(cube);
	checkedCubes.Add(cube);

	//A cute BFS
	//Grab cubes while we have any to check
	while (!cubesToCheck.IsEmpty()) {
		ACube* currentCube;
		cubesToCheck.Dequeue(currentCube);

		//Get surrounding neighbours
		auto neighborCubes = GetTouchingCubes(currentCube);

		for (ACube* neighborCube : neighborCubes)
		{
			if (neighborCube != nullptr) {

				//If we havent' checked them, process them
				if (!checkedCubes.Contains(neighborCube)) {
					// If it's the same color, add it to the list of same colored neighbors, and check from there too.
					if (neighborCube->GetColor() == cube->GetColor()) {
						groupCubes.Add(neighborCube);
						cubesToCheck.Enqueue(neighborCube);
					}
					checkedCubes.Add(neighborCube);
				}
			}
		}
	}

	return groupCubes;
}

uint32 APyramid::ExplodeCube(uint8 x, uint8 y) {
	return ExplodeCube(GetCubeAt(x, y));
}

uint32 APyramid::ExplodeCube(ACube* cube) {
	if (cube == nullptr)
		return 0;

	TSet<ACube*> cubes = GetGroupCubes(cube);
	//We explode every touching cube of the same color
	for (ACube* singleCube : cubes) {
		if (singleCube != nullptr) {
			SetCubeAt(singleCube->PyramidPosition.X, singleCube->PyramidPosition.Y, nullptr);
			singleCube->ExplodeCube();
			Client_ExplodeCube(singleCube);
		}
	}

	Multicast_RefreshCubesPositions();

	//Returns the score
	return GoodFib(cubes.Num());
}

void APyramid::Client_ExplodeCube_Implementation(ACube* cube) {
	SetCubeAt(cube->PyramidPosition.X, cube->PyramidPosition.Y, nullptr);
	cube->Client_ExplodeCube();
}

//Bad Fibonacci is bad
//uint32 APyramid::Fib(uint32 x) {
//	if (x == 0)
//		return 0;
//	if (x == 1)
//		return 1;
//	return Fib(x - 1) + Fib(x - 2);
//}

uint32 APyramid::GoodFib(uint16 x) {
	//First 3 cases are given
	if (x == 0) {
		return 0;
	}
	else if (x == 1 || x == 2) {
		return 1;
	}
	else {
		double a, b, i, save;
		//Start with 0 and 1.
		a = b = save = 0;
		a = 1;
		//We already have two first positions so we skip two loops.
		i = 2;
		while (i < x) {
			//We save the highest index value to place it on b after finding the sum of the values.
			save = a;
			//Find the new highest index value
			a = b + a;
			//load the old highest index value.
			b = save;
			i++;
		}
		return a + b;
	}
}

void APyramid::SetHighlightGroup(ACube* cube, bool on) {
	if (cube == nullptr)
		return;

	for (ACube* singleCube : GetGroupCubes(cube)) {
		singleCube->SetHighlight(on);
	}
}

void APyramid::Multicast_RefreshCubesPositions_Implementation() {

	//Check every position for cubes above it, and if it can fall, from the bottom to top
	for (uint8 y = 0; y < Height - 1; y++) {
		for (uint8 x = 0; x < (Height - y) * 2 - 1; x++) {

			FIntVector currentPos = FIntVector(x, y, 0);
			FIntVector cubeToDropPos = FIntVector(x + XDirection[Direction::N], y + YDirection[Direction::N], 0);

			//is there a cube above And Empty here?
			while (currentPos.Y >= 0 && !GetCubeAt(currentPos.X, currentPos.Y) && GetCubeAt(cubeToDropPos.X, cubeToDropPos.Y))
			{
				//If Cube at R and L are null, we drop it and check again.
				if ((!GetCubeAt(currentPos.X - 1, currentPos.Y) && !GetCubeAt(currentPos.X + 1, currentPos.Y))) {
					//Re-place the cube position.
					SetCubeAt(currentPos.X, currentPos.Y, GetCubeAt(cubeToDropPos.X, cubeToDropPos.Y));
					//Empty the previous cube position
					SetCubeAt(cubeToDropPos.X, cubeToDropPos.Y, nullptr);
				}

				//And check again lowering the positions
				cubeToDropPos = FIntVector(currentPos.X, currentPos.Y, 0);
				currentPos = FIntVector(currentPos.X + XDirection[Direction::S], currentPos.Y + YDirection[Direction::S], 0);
			}
		}
	}

	//As this is a multicast method, it executes on clients and server.
	//Making sure this is run only on the server.
	auto gm = GetWorld()->GetAuthGameMode<ANG_Test_MOGameMode>();
	if (gm) {
		bAnyCubeAlive = false;
		
		//look for any alive cube and stop if found any.
		for (uint16 i = 0; !bAnyCubeAlive && i < CubesArray.Num(); i++) {
			bAnyCubeAlive = bAnyCubeAlive || (CubesArray[i] != nullptr && CubesArray[i]->bAlive);
		}
	}

}
