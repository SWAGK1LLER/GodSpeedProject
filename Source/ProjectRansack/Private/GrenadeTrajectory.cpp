#include "GrenadeTrajectory.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Camera/CameraComponent.h"

UGrenadeTrajectory::UGrenadeTrajectory()
{
	PrimaryComponentTick.bCanEverTick = true;

	niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trajectory"));
}

void UGrenadeTrajectory::FinishAttachment(USceneComponent* root, UCameraComponent* pCamera)
{
	niagara->SetupAttachment(root);
	camera = pCamera;
}

void UGrenadeTrajectory::BeginPlay()
{
	Super::BeginPlay();
}

void UGrenadeTrajectory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	refreshCounter -= DeltaTime;
	timer -= DeltaTime;

	if (refresh)
		PredictGrenade(camera->GetComponentRotation().Pitch);
}

void UGrenadeTrajectory::PredictGrenade(float cameraRot)
{
	if (refreshCounter > 0)
		return;

	refreshCounter = 0.005;

	throwingVelo = throwVelocity * GetOwner()->GetActorForwardVector();
	throwingVelo.Z = throwVelocity.Z * (cameraRot / 8);

	FPredictProjectilePathParams param(radius, GetOwner()->GetActorLocation(), throwingVelo, simulationDuree);
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
	if (timer > 0)
		return;

	timer = 0.1;

	AGrenade* newGrenade = GetWorld()->GetWorld()->SpawnActor<AGrenade>(GrenadeClass, GetOwner()->GetActorLocation(), FRotator(), FActorSpawnParameters());
	newGrenade->SetVelocity(throwingVelo);
}

void UGrenadeTrajectory::MUlToggleVisibility_Implementation(bool visible)
{
	refresh = visible;

	if (!visible)
	{
		PredictionPositions.Empty();
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
	}

	//Spawn one grenade in the hand of the player
	//SetVisibility(visible);
}