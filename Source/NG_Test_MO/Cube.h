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
		//class UBoxComponent* BoxComp;


public:
	// Sets default values for this actor's properties
	ACube();

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UStaticMeshComponent* Mesh;

	class APyramid* Pyramid;
	
	FIntVector PyramidPosition;
	bool bAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	uint8 color;

	//void SetColor(int)
	//int8

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetColor(uint8 newColor);
	FORCEINLINE uint8 GetColor() { return color; }

	void AskSetHighlightGroup(bool on);
	void SetHighlight(bool on);
	void Init();
	void ExplodeCube();
	void AskExplodeCube();

};
