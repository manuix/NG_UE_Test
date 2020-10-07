// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"
#include "Pyramid.h"
#include "Materials/Material.h"
#include "Net/UnrealNetwork.h"
#include "MyGameState.h"

// Sets default values
ACube::ACube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Color = -1;
	Pyramid = nullptr;
	PyramidPosition.X = -1;
	PyramidPosition.Y = -1;


	SetReplicates(true);
	SetReplicateMovement(true);
}

//Variable replication
void ACube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACube, Pyramid);
	DOREPLIFETIME(ACube, PyramidPosition);
	DOREPLIFETIME(ACube, Color);

}

void ACube::OnRep_Color() {
	SetColor(Color);
}

void ACube::OnRep_Position() {
	if (Pyramid == nullptr)
		return;

	Pyramid->SetCubeAt(PyramidPosition.X, PyramidPosition.Y, this);
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

//0 = Red | 1 = Green | 2 = Blue | (-inf , 0) U (2, +inf) = ERROR
void ACube::SetColor(uint8 newColor) {
	Color = newColor;

	FVector newColorVector;
	switch (Color)
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

void ACube::ExplodeCube() {
	bAlive = false;
	Mesh->SetHiddenInGame(true);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(false);

}

void ACube::Client_ExplodeCube_Implementation() {
	bAlive = false;
	Mesh->SetHiddenInGame(true);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(false);
}