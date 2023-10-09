#include "ClaymoreComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"
#include "GamePlayerController.h"
#include "Claymore.h"
#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include <Kismet/KismetMathLibrary.h>

UClaymoreComponent::UClaymoreComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	preview = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClaymorePreviewMesh"));
	preview->SetVisibility(false);
}

void UClaymoreComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UClaymoreComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isActive)
		updatePosing(pawn->GetActorLocation(), pawn->cameraComponent->camera->GetForwardVector());
}

void UClaymoreComponent::updatePosing(FVector CamLocation, FVector CamForward)
{
	FHitResult Hit(ForceInit);

	FVector begin = pawn->cameraComponent->camera->GetComponentLocation();
	FVector forward = UKismetMathLibrary::GetForwardVector(pawn->cameraComponent->camera->GetComponentRotation());
	FVector LineTraceEnd = begin + (forward * viewReach);

	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(Hit, begin, LineTraceEnd, ECC_WorldDynamic, CollisionParams);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.GetComponent() == preview || Hit.GetActor()->IsA(ABase3C::StaticClass()))
			return;

		if (Cast<AClaymore>(Hit.GetActor()) != nullptr)
			return;

		//MUlToggleVisibility(true);

		MeshNormal = Hit.ImpactNormal;
		Rotation = FRotationMatrix::MakeFromX(MeshNormal).Rotator();
		FRotator Offset = FRotator(90, 180, 0);
		Rotation += Offset;

		Location = Hit.Location + (MeshNormal * 5);

		preview->SetWorldLocation(Location);
		preview->SetWorldRotation(Rotation);
		return;
	}

	//MUlToggleVisibility(false);
}

void UClaymoreComponent::MUlToggleVisibility_Implementation(bool visible)
{
	if (!preview)
		return;

	isActive = visible;
	preview->SetVisibility(visible);
}

void UClaymoreComponent::MUlFire_Implementation()
{
	if (controller != nullptr)
		controller->SpawnClaymore(ClaymoreTospawn, Location, Rotation, pawn);
}

void UClaymoreComponent::UpdateUI_Implementation()
{
	pawn->WidgetUI->ShowClemore();
}