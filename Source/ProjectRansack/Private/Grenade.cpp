#include "Grenade.h"
#include "Components/StaticMeshComponent.h"

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("mesh"));
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