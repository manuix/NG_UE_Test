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

	//UPROPERTY()
	//UStaticMeshComponent* Root;

public:
	// Sets default values for this actor's properties
	APyramid();

	//UPROPERTY(EditAnywhere, Category = "PyramidSettings")
	TSubclassOf<class ACube> CubeClass;
	//UPROPERTY()
	//UChildActorComponent* container;

	UPROPERTY(EditAnywhere, Category = "PyramidSettings")
	uint8 Height;
	FORCEINLINE bool IsAnyCubeAlive() { return bAnyCubeAlive; };

	//UPROPERTY(VisibleAnywhere, Category = "PyramidSettings")

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//TODO
	//We need to pool dead Cubes
	TArray<ACube*> CubesArray;
	//TQueue<ACube*> deadCubes;

	bool bAnyCubeAlive;

	bool IsCoordinateWithinRange(int16 x, int16 y);
	void RefreshCubesPositions();

	//uint32 Fib(uint32 x);
	uint32 GoodFib(uint16 x);

public:
	//Prolly should be private
	TArray<ACube*> GetTouchingCubes(uint8 x, uint8 y);
	TArray<ACube*> GetTouchingCubes(ACube* cube);
	TSet<ACube*> GetGroupCubes(uint8 x, uint8 y);
	TSet<ACube*> GetGroupCubes(ACube* cube);

	//[deprecated]
	uint16 GetPositionInArray(uint8 x, uint8 y);
	ACube* GetCubeAt(uint8 x, uint8 y);
	void SetCubeAt(uint8 x, uint8 y, ACube* cube);


	void GeneratePyramid();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	uint32 ExplodeCube(uint8 x, uint8  y);
	uint32 ExplodeCube(ACube* cube);
	void SetHighlightGroup(ACube* cube, bool on);



};
