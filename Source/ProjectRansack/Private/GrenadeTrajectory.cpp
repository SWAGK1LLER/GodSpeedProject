#include "GrenadeTrajectory.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include <Thief.h>
#include <GamePlayerController.h>
#include "Engine/Texture2D.h"
#include "Equipement.h"

UGrenadeTrajectory::UGrenadeTrajectory()
{
	PrimaryComponentTick.bCanEverTick = true;

	niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trajectory"));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
}

void UGrenadeTrajectory::FinishAttachment(USceneComponent* root, UCameraComponent* pCamera)
{
	niagara->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
	camera = pCamera;

	mesh->AttachToComponent(root, FAttachmentTransformRules::SnapToTargetNotIncludingScale, (root, FName("RightHandSocket")));
}

void UGrenadeTrajectory::SetIslocalController(bool local)
{
	isLocalComp = local;
}

void UGrenadeTrajectory::BeginPlay()
{
	Super::BeginPlay();

	mesh->SetVisibility(false);
	PredictionPositions.Empty();
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
}

void UGrenadeTrajectory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!isActive)
		return;

	refreshCounter -= DeltaTime;
	timer -= DeltaTime;

	//Calculate throwing velo each frame
	throwingVelo = throwVelocity * GetOwner()->GetActorForwardVector();
	throwingVelo.Z = throwVelocity.Z * (camera->GetComponentRotation().Pitch / 8);

	if (refresh)
		PredictGrenade();
	/*else
	{
		PredictionPositions.Empty();
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
	}*/
}

void UGrenadeTrajectory::PredictGrenade()
{
	if (refreshCounter > 0)
		return;

	refreshCounter = 0.005;

	FPredictProjectilePathParams param;
	if (isThrowing)
		param = FPredictProjectilePathParams(radius, throwPosition + predictionPathOffset, throwingVelo, simulationDuree);
	else
		param = FPredictProjectilePathParams(radius, mesh->GetComponentLocation() + predictionPathOffset, throwingVelo, simulationDuree);
	
	FPredictProjectilePathResult result;

	UGameplayStatics::PredictProjectilePath(GetWorld(), param, result);

	//Send position to niagara
	PredictionPositions.Empty();
	for (FPredictProjectilePathPointData& data : result.PathData)
		PredictionPositions.Add(data.Location);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
}

void UGrenadeTrajectory::ThrowGrenade()
{
	if (controller != nullptr && controller->IsLocalPlayerController())
	{
		controller->SpawnGrenade(pawn, mesh->GetComponentLocation(), throwingVelo, *GrenadeClass);
	}
}

void UGrenadeTrajectory::MUlToggleVisibility_Implementation(bool visible)
{
	isActive = visible;
	mesh->SetVisibility(visible);

	if (!isLocalComp)
		return;

	CLTogglePredictPath(visible);
}

void UGrenadeTrajectory::CLTogglePredictPath(bool visible)
{
	refresh = visible;
	if (!refresh)
	{
		PredictionPositions.Empty();
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
	}
}

void UGrenadeTrajectory::MUlFire_Implementation()
{
	if (timer > 0 || *currentAmmo == 0)
		return;

	timer = 0.1;
	(*currentAmmo)--;

	isThrowing = true;
	throwPosition = mesh->GetComponentLocation();

	pawn->MUlThrowGrenade();
}

void UGrenadeTrajectory::EndThrow()
{
	isThrowing = false;
}

void UGrenadeTrajectory::UpdateUI_Implementation()
{
	pawn->WidgetUI->ShowGrenade(uiTexture);
}

bool UGrenadeTrajectory::IsSameGrenadeClass()
{
	return CurrentGrenadeClass == previousGrenadeClass;
}