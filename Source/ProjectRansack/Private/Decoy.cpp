#include "Decoy.h"
#include "Thief.h"
#include "Camera/CameraComponent.h"
#include "CameraComp.h"
#include "DecoyActor.h"
#include "GameFramework/PawnMovementComponent.h"

UDecoy::UDecoy()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDecoy::SetupComp(AThief* pThief)
{
	owner = pThief;
}

void UDecoy::BeginPlay()
{
	Super::BeginPlay();
}

void UDecoy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	timer -= DeltaTime;
}

void UDecoy::SpawnDecoy()
{
	if (timer > 0)
		return;

	timer = 0.2f;

	FVector velocityoffSet = owner->GetActorForwardVector() * 50;

	FRotator cam = owner->cameraComponent->camera->GetForwardVector().Rotation();
	cam.Roll = 0;
	cam.Pitch = 0;

	ADecoyActor* actor = GetWorld()->GetWorld()->SpawnActor<ADecoyActor>(DecoyActorClass, owner->GetActorLocation() + velocityoffSet, cam, FActorSpawnParameters());
	actor->GetMovementComponent()->Velocity = owner->GetMovementComponent()->Velocity;
}