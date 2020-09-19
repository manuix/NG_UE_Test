// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"
#include "Pyramid.h"
#include "Materials/Material.h"

// Sets default values
ACube::ACube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	color = 0;
	Pyramid = nullptr;
}

// Called when the game starts or when spawned
void ACube::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACube::SetColor(uint8 newColor) {
	color = newColor;

	//FColor
	FVector newColorVector;
	switch (color)
	{
	case 0:
		newColorVector = FVector(.8f, .2f, .2f);
		break;
	case 1:
		newColorVector = FVector(.2f, .8f, .2f);
		break;
	case 2:
		newColorVector = FVector(.2f, .2f, .8f);
		break;
	default:
		newColorVector = FVector(.8f, .2f, .8f);
		break;
	}

	Mesh->SetVectorParameterValueOnMaterials("Color", newColorVector);
}


void ACube::SetHighlight(bool on) {
	Mesh->SetScalarParameterValueOnMaterials("Highlight", on);
}


void ACube::Spawn() {
	bAlive = true;
	Mesh->SetHiddenInGame(false);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetActorTickEnabled(true);
	
}

void ACube::AskExplodeCube() {
	if (Pyramid != nullptr) {
		Pyramid->ExplodeCube(this);
	}
}

void ACube::ExplodeCube() {

	bAlive = false;
	Mesh->SetHiddenInGame(true);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(false);

	//Feedback maybe?
}