// Copyright Epic Games, Inc. All Rights Reserved.

#include "NG_Test_MOCharacter.h"
#include "NG_Test_MOProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

#include "GameFramework/PlayerState.h"
#include "Math/Color.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"
#include "Components/Border.h"

#include "Styling/SlateColor.h"
#include "NG_Test_MOGameMode.h"
#include "MyGameState.h"
#include "Cube.h"
#include "Pyramid.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ANG_Test_MOCharacter

ANG_Test_MOCharacter::ANG_Test_MOCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
	SetReplicates(true);
	SetReplicateMovement(true);

}


//////////////////////////////////////////////////////////////////////////
// Input

void ANG_Test_MOCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANG_Test_MOCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ANG_Test_MOCharacter::ClickOnCube);
	//Debug cube
	//PlayerInputComponent->BindAction("Fire2", IE_Pressed, this, &ANG_Test_MOCharacter::DebugCube);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANG_Test_MOCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ANG_Test_MOCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANG_Test_MOCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANG_Test_MOCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANG_Test_MOCharacter::LookUpAtRate);
}

void ANG_Test_MOCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ANG_Test_MOProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ANG_Test_MOProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ANG_Test_MOCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANG_Test_MOCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ANG_Test_MOCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ANG_Test_MOCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ANG_Test_MOCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANG_Test_MOCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ANG_Test_MOCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANG_Test_MOCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ANG_Test_MOCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ANG_Test_MOCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ANG_Test_MOCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ANG_Test_MOCharacter::TouchUpdate);
		return true;
	}

	return false;
}



void ANG_Test_MOCharacter::BeginPlay()
{

	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}


}



void ANG_Test_MOCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled()) {
		CheckForCubes();
	}
}



void ANG_Test_MOCharacter::CheckForCubes() {
	FHitResult THit;

	FVector start = FirstPersonCameraComponent->GetComponentLocation();
	FVector end = start + FirstPersonCameraComponent->GetForwardVector() * 10000;
	FCollisionQueryParams CollisionParams;


	GetWorld()->LineTraceSingleByChannel(THit, start, end, ECC_Visibility, CollisionParams);
	// Am I looking at a cube?
	ACube* hitCube = (Cast<ACube>(THit.Actor));
	SetCubeImLookingAt(hitCube);
}

void ANG_Test_MOCharacter::SetCubeImLookingAt(ACube* newCube) {
	/*
	* We turn off the highlight on any currently looked at cube
	* Then we turn it on on any cube that we might be lookign at now.
	*/
	if (CubeImLookingAt != nullptr) {
		if (CubeImLookingAt != newCube) {
			if (CubeImLookingAt->Pyramid) {
				for (auto sCube : CubeImLookingAt->Pyramid->GetGroupCubes(CubeImLookingAt)) {
					sCube->SetHighlight(false);
				}
			}
			CubeImLookingAt->SetHighlight(false);
		}
	}

	CubeImLookingAt = newCube;
	if (newCube != nullptr) {
		if (newCube->Pyramid) {
			for (auto sCube : newCube->Pyramid->GetGroupCubes(CubeImLookingAt)) {
				sCube->SetHighlight(true);
			}
		}
		newCube->SetHighlight(true);
	}
}

//If someone tries to destroy a cube whtn it's not their turn, should it disconnect them? not so sure. 
//The server just checks if it's their turn and then explodes it or not accordingly
bool ANG_Test_MOCharacter::Server_AskClickOnCube_Validate(APlayerController* pController, ACube* cube) {
	return true;
}

//TODO: Move this to the GameMode? IDK. this runs only on the server, but I get the feeling that if placed in the gamemode script the code would look more neat.
void ANG_Test_MOCharacter::Server_AskClickOnCube_Implementation(APlayerController* pController, ACube* cube) {

	auto gameState = GetWorld()->GetGameState<AMyGameState>();

	//Checking it's my turn, otherwise, do nothing.
	if (gameState->CurrentTurn != pController->GetPlayerState<APlayerState>()) {
		return;
	}

	//Explode the cube and add the score they get.
	auto score = cube->Pyramid->ExplodeCube(cube);
	if (pController != nullptr && pController->PlayerState != nullptr)
		pController->PlayerState->SetScore(pController->PlayerState->GetScore() + score);

	gameState->SetNextTurn();
}

void ANG_Test_MOCharacter::ClickOnCube() {
	if (CubeImLookingAt != nullptr) {
		auto gs = GetWorld()->GetGameState<AMyGameState>();
		if (gs && gs->CurrentTurn == GetPlayerState()) {
			Server_AskClickOnCube((APlayerController*)Controller, CubeImLookingAt);
		}
	}
}

//Array with the direction names for debugging
const TArray<FString> DirectionNames = { ("N"), ("NE"),  ("E"), ("SE"),  ("S"), ("SW"),  ("W"), ("NW") };
void ANG_Test_MOCharacter::DebugCube() {
	if (CubeImLookingAt != nullptr) {
		auto cubes = CubeImLookingAt->Pyramid->GetTouchingCubes(CubeImLookingAt);
		for (uint8 i = 0; i < 8; i++) {
			if (GEngine) {

				if (cubes[i]) {
					FString colorName = ("");
					//Should be Soft-coded (Des hardcodear)
					switch (cubes[i]->GetColor())
					{
					case 0:
						colorName = ("Red");
						break;
					case 1:
						colorName = ("Green");
						break;
					case 2:
						colorName = ("Blue");
						break;
					default:
						break;
					}
					GEngine->AddOnScreenDebugMessage(50 + i, 20, FColor::Yellow, FString::Printf(TEXT("Cube at %s is %s"), *DirectionNames[i], *colorName));
				}
				else {
					GEngine->AddOnScreenDebugMessage(50 + i, 20, FColor::Orange, FString::Printf(TEXT("No Cube at %s"), *DirectionNames[i]));
				}
			}
		}
	}
}
