#include "Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

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

void AGrenade::SetVelocity(FVector velocity)
{
	mesh->SetPhysicsLinearVelocity(velocity);
}