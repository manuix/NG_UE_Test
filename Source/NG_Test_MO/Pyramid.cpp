// Fill out your copyright notice in the Description page of Project Settings.


#include "Pyramid.h"
#include "Cube.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
APyramid::APyramid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Root = CreateDefaultSubobject<USceneComponent>("Root");
	//Root = CreateDefaultSubobject<UStaticMeshComponent>("Root");
	//RootComponent = Root;

	//container = CreateDefaultSubobject<UChildActorComponent>(TEXT("Container"));

	Height = 7;


}

// Called when the game starts or when spawned
void APyramid::BeginPlay()
{
	Super::BeginPlay();

	cubesArray.Init(nullptr, Height * Height);

	GeneratePyramid();

	if (GEngine) {

		auto time = FDateTime::Now();
		
	}

}

// Called every frame
void APyramid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UFUNCTION(CallInEditor, Category = "PyramidSettings")
void APyramid::GeneratePyramid() {

	bIsAnyCubeAlive = true;

	//an array and stuff
	//A for

	for (uint8 y = 0; y < Height; y++)
	{
		for (uint8 x = 0; x < Height - y; x++)
		{
			uint16 PositionInArray = GetPositionInArray(x * 2, y);
			//TEXT("Cube(" + x + ";" + y + ")")
			ACube* cube;
			if (!deadCubes.IsEmpty()) {
				deadCubes.Dequeue(cube);
				//Init
			}
			else {
				cube = GetWorld()->SpawnActor<ACube>(CubeClass);
				//Actualy Spawining it
			}
			cube->SetOwner(this);
			cube->SetActorLabel(FString::Printf(TEXT("Cube %i | %i"), x * 2, y));
			cube->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			cube->SetActorRelativeLocation(GetActorLocation() + FVector(0, 110 * x + 55 * y, 110 * y), false, nullptr, ETeleportType::ResetPhysics);
			/*if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, cube->GetName());*/
			cube->PyramidPosition = FIntVector(x * 2, y, 0);
			//???
			//cube->Mesh->SetupAttachment(RootComponent);

			//Locking the Y Position of the cubes
			cube->Mesh->BodyInstance.bLockYTranslation = true;
			cube->Mesh->BodyInstance.SetDOFLock(EDOFMode::SixDOF);

			cube->SetColor(FMath::RandRange(0, 2));
			//cube->SetColor(0);

			cube->Pyramid = this;
			cube->Init();

			cubesArray[PositionInArray] = cube;

		}

	}

}

//Can return -1 if out of range.
uint16 APyramid::GetPositionInArray(uint8 x, uint8 y) {

	if (!ISCoordinateWithinRange(x, y)) {
		//return -1;
		throw std::out_of_range("Coordinates are out of range.");
	}

	uint16 ret = 0;

	for (uint8 i = 0; i < y; i++)
	{
		ret += (Height - i) * 2 - 1;
	}

	ret += x;

	return ret;
}

//Can return nullptr if out of range.
ACube* APyramid::GetCubeAt(uint8 x, uint8 y) {
	ACube* cube = nullptr;

	if (!ISCoordinateWithinRange(x, y)) {
		return cube;
	}

	return cubesArray[GetPositionInArray(x, y)];
}

//Won't do anything if out of range. Sets the local Cubes position too.
void APyramid::SetCubeAt(uint8 x, uint8 y, ACube* cube) {
	if (ISCoordinateWithinRange(x, y)) {
		cubesArray[GetPositionInArray(x, y)] = cube;
		if (cube != nullptr) {
			cube->PyramidPosition = FIntVector(x, y, 0);
			/*if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, cube->PyramidPosition.ToString());*/
		}/*
		else {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, "null cube.");
		}*/
	}/*
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, "Out of range");
	}*/
}


bool APyramid::ISCoordinateWithinRange(int16 x, int16 y) {
	return
		y < Height&&
		y >= 0 &&
		x < (Height - y) * 2 - 1 &&
		x >= 0
		;
}


//								   N, NE,  E, SE,  S, SW,  W, NW
const TArray<int8> XDirection = { -1,  0,  2,  2,  1,  0, -2, -2 };
const TArray<int8> YDirection = { 1,  1,  0, -1, -1, -1,  0,  1 };
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
	return GetTouchingCubes(cube->PyramidPosition.X, cube->PyramidPosition.Y);
}

TSet<ACube*> APyramid::GetGroupCubes(uint8 x, uint8 y) {
	return GetGroupCubes(cubesArray[GetPositionInArray(x, y)]);
}

TSet<ACube*> APyramid::GetGroupCubes(ACube* cube) {

	TSet<ACube*> groupCubes;

	if (cube == nullptr)
		return groupCubes;

	//sameColorNeighborCubes.Append(Pyramid->GetNeighboringCubes(this));
	TSet<ACube*> checkedCubes;
	TQueue<ACube*> cubesToCheck;

	//This cube is the starting point.
	groupCubes.Add(cube);
	cubesToCheck.Enqueue(cube);
	checkedCubes.Add(cube);

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

	/*if (GEngine != nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString::Printf(TEXT("Destroyed %s"), *cube->GetName()));
	}*/
	TSet<ACube*> cubes = GetGroupCubes(cube);
	for (ACube* singleCube : cubes) {
		deadCubes.Enqueue(singleCube);
		SetCubeAt(singleCube->PyramidPosition.X, singleCube->PyramidPosition.Y, nullptr);
		singleCube->ExplodeCube();
	}

	RefreshCubesPositions();

	return GoodFib(cubes.Num());
}

//uint32 APyramid::Fib(uint32 x) {
//	if (x == 0)
//		return 0;
//	if (x == 1)
//		return 1;
//	return Fib(x - 1) + Fib(x - 2);
//}

uint32 APyramid::GoodFib(uint16 x) {
	if (x == 0) {
		return 0;
	}
	else if (x == 1) {
		return 1;
	}
	else {
		double a, b, i, save;
		a = b = i = save = 0;
		a = 1;
		i = 2;
		while (i < x) {
			save = a;
			a = b + a;
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

void APyramid::RefreshCubesPositions() {

	bIsAnyCubeAlive = false;

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(11, 3, FColor::Green, "Refreshing...");

	//bool bHasToRefresh = true;
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::White, "Hi");*/


		//Check every position for cubes above it, and if it can fall, from the bottom to top
	for (uint8 y = 0; y < Height - 1; y++) {
		for (uint8 x = 0; x < (Height - y) * 2 - 1; x++) {

			FIntVector currentPos = FIntVector(x, y, 0);
			FIntVector toDropPos = FIntVector(x + XDirection[Direction::N], y + YDirection[Direction::N], 0);


			bIsAnyCubeAlive = bIsAnyCubeAlive || GetCubeAt(currentPos.X, currentPos.Y) != nullptr;

			//is there a cube above And Empty here?
			while (currentPos.Y >= 0 && !GetCubeAt(currentPos.X, currentPos.Y) && GetCubeAt(toDropPos.X, toDropPos.Y))
			{
				//ACube* RCube, * LCube;
				//RCube = GetCubeAt(currentPos.X - 1, currentPos.Y);
				//LCube = GetCubeAt(currentPos.X + 1, currentPos.Y);

				//If R and L are null, we drop it and check again.
				if (!GetCubeAt(currentPos.X - 1, currentPos.Y) && !GetCubeAt(currentPos.X + 1, currentPos.Y)) {
					/*if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue, FString::Printf(TEXT("Setting Cube from [X: %i ; Y: %i] to [X: %i ; Y: %i]"), x - 1, y + 1, x, y));
					*/
					//Empty the previous cube position
					SetCubeAt(currentPos.X, currentPos.Y, GetCubeAt(toDropPos.X, toDropPos.Y));
					SetCubeAt(toDropPos.X, toDropPos.Y, nullptr);
				}
				//We drop
				//And check again

				toDropPos = FIntVector(currentPos.X, currentPos.Y, 0);
				currentPos = FIntVector(currentPos.X + XDirection[Direction::S], currentPos.Y + YDirection[Direction::S], 0);
			}

			//Old Top to bottom way

			//ACube* cube, * SWCube, * SCube, * SECube;
			//cube = cubesArray[GetPositionInArray(x, y)];
			//if (cube != nullptr) {
			//	SWCube = cubesArray[GetPositionInArray(x, y - 1)];
			//	SCube = cubesArray[GetPositionInArray(x + 1, y - 1)];
			//	SECube = cubesArray[GetPositionInArray(x + 2, y - 1)];

			//	if (SWCube == nullptr && SCube == nullptr && SECube == nullptr) {
			//		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString::Printf(TEXT("Setting Cube from [X: %i ; Y: %i] to [X: %i ; Y: %i]"), x, y, x + 1, y - 1));
			//		cubesArray[GetPositionInArray(x, y)] = nullptr;
			//		cubesArray[GetPositionInArray(x + 1, y - 1)] = cube;
			//		cube->PyramidPosition.Set(x + 1, y - 1);

			//	}
			//}
		}
	}


}