#include "CameraCompThief.h"
#include "Camera/CameraComponent.h"
#include <ProjectRansack/Public/Base3C.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool SameSign(float x, float y)
{
	return ((x < 0) == (y < 0));
}

UCameraCompThief::UCameraCompThief()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	camera->bUsePawnControlRotation = false;

	ABase3C* owner = Cast<ABase3C>(GetOwner());
	if (owner == nullptr)
		return;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	owner->bUseControllerRotationPitch = false;
	owner->bUseControllerRotationYaw = false;
	owner->bUseControllerRotationRoll = false;

	// Configure character movement
	owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	owner->GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
}

void UCameraCompThief::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UCameraComp::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldTurn)
	{
		ACharacter* Character = CastChecked<ACharacter>(GetOwner());
		Character->SetActorRotation(FMath::Lerp(Character->GetActorRotation(), finalRotation, 0.09f));

		if (Character->GetActorRotation() == finalRotation)
			ShouldTurn = false;
	}
}

void UCameraCompThief::SetupCamera(USceneComponent* root)
{
	CameraBoom->SetupAttachment(root, "NeckSocket");
	camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	rootMesh = root;
}

void UCameraCompThief::look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (ACharacter* Character = CastChecked<ACharacter>(GetOwner()))
	{
		Character->AddControllerYawInput(LookAxisVector.X);
		Character->AddControllerPitchInput(LookAxisVector.Y);
		
		RotatePlayer(Character);
	}
}

void UCameraCompThief::RotatePlayer(ACharacter* Character)
{
	FVector CameraLocation = camera->GetComponentLocation();
	FVector CameraForwardVector = camera->GetForwardVector();
	FVector ToPlayerCharacter = Character->GetActorForwardVector();

	CameraForwardVector.Normalize();
	ToPlayerCharacter.Normalize();

	if (FVector::DotProduct(CameraForwardVector, ToPlayerCharacter) < 0.5f)
	{
		ShouldTurn = true;

		//To do : lerp rotation
		FRotator rotation = Character->GetViewRotation();
		rotation.Pitch = 0;
		rotation.Roll = 0;

		finalRotation = rotation;

		//Character->SetActorRotation(rotation);
	}
}