#include "Decoy.h"
#include "Base3C.h"
#include "Camera/CameraComponent.h"
#include "CameraComp.h"
#include "DecoyActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include <GamePlayerController.h>

UDecoy::UDecoy()
{
	PrimaryComponentTick.bCanEverTick = false;
	owner = Cast<ABase3C>(GetOwner());
}

void UDecoy::BeginPlay()
{
	Super::BeginPlay();

	AController* controller = owner->GetController();
	if (controller == nullptr || !controller->IsLocalPlayerController())
		return;

	pcCache = Cast<AGamePlayerController>(controller);
}

void UDecoy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDecoy::SpawnDecoy()
{
	if (pcCache == nullptr || !pcCache->IsLocalPlayerController())
		return;
	FVector velocityoffSet = owner->GetActorForwardVector() * 50;

	FRotator cam = owner->cameraComponent->camera->GetForwardVector().Rotation();
	cam.Roll = 0;
	cam.Pitch = 0;

	pcCache->SpawnDecoy(DecoyActorClass, owner->GetActorLocation() + velocityoffSet, cam);
}

void UDecoy::MUlFire_Implementation()
{
	SpawnDecoy();
}

void UDecoy::UpdateUI_Implementation()
{
	owner->WidgetUI->ShowDecoy();
}