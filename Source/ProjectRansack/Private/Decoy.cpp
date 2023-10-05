#include "Decoy.h"
#include "Thief.h"
#include "Camera/CameraComponent.h"
#include "CameraComp.h"
#include "DecoyActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include <GamePlayerController.h>

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
	/*if (timer > 0)
		return;*/
	
	//timer = 0.2f;
	
	AController* controller = owner->GetController();
	if (controller == nullptr || !controller->IsLocalPlayerController())
		return;


	FVector velocityoffSet = owner->GetActorForwardVector() * 50;

	FRotator cam = owner->cameraComponent->camera->GetForwardVector().Rotation();
	cam.Roll = 0;
	cam.Pitch = 0;

	AGamePlayerController* pc = Cast<AGamePlayerController>(controller);
	pc->SpawnDecoy(DecoyActorClass, owner->GetActorLocation() + velocityoffSet, cam);

	/*ADecoyActor* actor = GetWorld()->GetWorld()->SpawnActor<ADecoyActor>(DecoyActorClass, owner->GetActorLocation() + velocityoffSet, cam, FActorSpawnParameters());
	actor->GetMovementComponent()->Velocity = owner->GetMovementComponent()->Velocity;*/
}