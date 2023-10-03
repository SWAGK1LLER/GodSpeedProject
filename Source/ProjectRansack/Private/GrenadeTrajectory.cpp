#include "GrenadeTrajectory.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

UGrenadeTrajectory::UGrenadeTrajectory()
{
	PrimaryComponentTick.bCanEverTick = true;

	niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trajectory"));
}

void UGrenadeTrajectory::FinishAttachment(USceneComponent* root)
{
	niagara->SetupAttachment(root);
}

void UGrenadeTrajectory::BeginPlay()
{
	Super::BeginPlay();
}

void UGrenadeTrajectory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	refreshCounter -= DeltaTime;
}

void UGrenadeTrajectory::PredictGrenade(float cameraRot)
{
	if (refreshCounter > 0)
		return;

	refreshCounter = 0.05;

	throwingVelo = throwVelocity * GetOwner()->GetActorForwardVector();
	throwingVelo.Z = throwVelocity.Z * (cameraRot / 8);

	FPredictProjectilePathParams param(radius, GetOwner()->GetActorLocation(), throwingVelo, simulationDuree);
	//param.DrawDebugType = EDrawDebugTrace::Persistent;
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
	
}