#include "CameraCompThief.h"
#include "Camera/CameraComponent.h"
#include <ProjectRansack/Public/Base3C.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GamePlayerController.h"

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

	ACharacter* Character = CastChecked<ACharacter>(GetOwner());

	speed = Character->GetCharacterMovement()->Velocity.Size2D();
	
	if (speed > 3.0f && Character->GetCharacterMovement()->GetLastInputVector() != FVector(0, 0, 0))
	{
		FRotator rotation = Character->GetViewRotation();
		rotation.Pitch = 0;
		rotation.Roll = 0;

		Character->SetActorRotation(rotation);
		Cast<AGamePlayerController>(Character->GetController())->MUlSetRot(this, rotation);

		//Character->bUseControllerRotationYaw = true;
		ShouldTurn = false;
	}
	//else
	//	Character->bUseControllerRotationYaw = false;
	
	if (ShouldTurn)
	{
		Character->SetActorRotation(FMath::Lerp(Character->GetActorRotation(), finalRotation, SnapSpeed));

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

	if (FVector::DotProduct(CameraForwardVector, ToPlayerCharacter) < SnapThreshold)
	{
		ShouldTurn = true;

		FRotator rotation = Character->GetViewRotation();
		rotation.Pitch = 0;
		rotation.Roll = 0;

		finalRotation = rotation;

		ACharacter* Character = CastChecked<ACharacter>(GetOwner());
		Cast<AGamePlayerController>(Character->GetController())->MUlSetLerpRot(this, finalRotation);
	}
}

void UCameraCompThief::MUlSetLerpRot_Implementation(FRotator rot)
{
	ShouldTurn = true;
	finalRotation = rot;
}

void UCameraCompThief::MUlSetRot_Implementation(FRotator rot)
{
	ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	Character->SetActorRotation(rot);
}