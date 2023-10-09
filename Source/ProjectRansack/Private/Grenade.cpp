#include "Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("mesh"));
}

void AGrenade::SetThrower(ABase3C* pOwner)
{
	owner = pOwner;
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	mesh->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);
	mesh->SetSimulatePhysics(true);
}

void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrenade::SetVelocityLocal(FVector velocity)
{
	mesh->SetPhysicsLinearVelocity(velocity);
}

void AGrenade::MUlSetVelocity_Implementation(ABase3C* pPawn, FVector velo)
{
	SetThrower(pPawn);
	SetVelocityLocal(velo);
}