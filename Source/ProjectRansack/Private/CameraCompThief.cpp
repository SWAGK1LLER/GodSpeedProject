#include "CameraCompThief.h"
#include "Camera/CameraComponent.h"
#include <ProjectRansack/Public/Base3C.h>
#include "GameFramework/CharacterMovementComponent.h"

UCameraCompThief::UCameraCompThief()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	
	CameraBoom->TargetArmLength = 400.0f;
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
	owner->GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
}

void UCameraCompThief::SetupCamera(USceneComponent* root)
{
	CameraBoom->SetupAttachment(root, "NeckSocket");
	camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void UCameraCompThief::look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (ACharacter* Character = CastChecked<ACharacter>(GetOwner()))
	{
		Character->AddControllerYawInput(LookAxisVector.X);
		Character->AddControllerPitchInput(LookAxisVector.Y);
	}
}