#include "Decoy.h"
#include "Base3C.h"
#include "Camera/CameraComponent.h"
#include "CameraComp.h"
#include "DecoyActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/SkeletalMesh.h"
#include <GamePlayerController.h>

UDecoy::UDecoy()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDecoy::BeginPlay()
{
	Super::BeginPlay();
}

void UDecoy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDecoy::FinishSetup()
{
	originalMesh = pawn->GetMesh()->GetSkeletalMeshAsset();
	originalAnim = pawn->GetMesh()->GetAnimInstance();
}

void UDecoy::SpawnDecoy()
{
	if (controller == nullptr || !controller->IsLocalPlayerController())
		return;

	FVector velocityoffSet = pawn->GetActorForwardVector() * 50;

	FRotator cam = pawn->cameraComponent->camera->GetForwardVector().Rotation();
	cam.Roll = 0;
	cam.Pitch = 0;

	controller->SpawnDecoy(DecoyActorClass, originalMesh, originalAnim->GetClass(), pawn->GetActorLocation() + velocityoffSet, cam);
}

void UDecoy::MUlFire_Implementation()
{
	SpawnDecoy();
}

void UDecoy::UpdateUI_Implementation()
{
	pawn->WidgetUI->ShowDecoy();
}