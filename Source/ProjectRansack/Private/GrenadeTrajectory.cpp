#include "GrenadeTrajectory.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include <Thief.h>
#include <GamePlayerController.h>

UGrenadeTrajectory::UGrenadeTrajectory()
{
	PrimaryComponentTick.bCanEverTick = true;

	niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trajectory"));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshExposed"));
}

void UGrenadeTrajectory::FinishAttachment(USceneComponent* root, UCameraComponent* pCamera)
{
	niagara->SetupAttachment(root);
	camera = pCamera;

	mesh->SetupAttachment(root, FName("RightHandSocket"));
}

void UGrenadeTrajectory::SetIslocalController(bool local)
{
	isLocalComp = local;
}

void UGrenadeTrajectory::BeginPlay()
{
	Super::BeginPlay();

	mesh->SetVisibility(false);
}

void UGrenadeTrajectory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	refreshCounter -= DeltaTime;
	timer -= DeltaTime;

	//Calculate throwing velo each frame
	throwingVelo = throwVelocity * GetOwner()->GetActorForwardVector();
	throwingVelo.Z = throwVelocity.Z * (camera->GetComponentRotation().Pitch / 8);

	if (refresh && isLocalComp)
		PredictGrenade();
	else if (isLocalComp)
	{
		PredictionPositions.Empty();
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(niagara, FName("Positions"), PredictionPositions);
	}
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
	if (timer > 0 || ammo == 0)
		return;

	timer = 0.1;
	ammo--;

	AGrenade* newGrenade = GetWorld()->GetWorld()->SpawnActor<AGrenade>(GrenadeClass, mesh->GetComponentLocation(), FRotator(), FActorSpawnParameters());
	newGrenade->SetVelocity(throwingVelo);
}

void UGrenadeTrajectory::MUlToggleVisibility_Implementation(bool visible)
{
	mesh->SetVisibility(visible);

	APawn* owner = Cast<APawn>(GetOwner());
	AGamePlayerController* playerController = Cast<AGamePlayerController>(owner->GetController());
	if (playerController == nullptr)
		return;

	if (!playerController->IsLocalPlayerController())
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
	isThrowing = true;
	throwPosition = mesh->GetComponentLocation();

	Cast<AThief>(GetOwner())->MUlThrowGrenade();
}

void UGrenadeTrajectory::EndThrow()
{
	isThrowing = false;
}

void UGrenadeTrajectory::UpdateUI_Implementation()
{
	Cast<ABase3C>(GetOwner())->WidgetUI->ShowGrenade();
}