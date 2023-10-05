#include "Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GamePlayerController.h"
#include "GenericParticleSystemComponent.h"
#include <Kismet/GameplayStatics.h>

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

void AGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (counterStarted)
		return;

	counterStarted = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
		Explose();
	}), timeBeforeExploseOnContact, false);
}

void AGrenade::Explose()
{
	SetLifeSpan(effectDuration);

	UGenericParticleSystemComponent* particle = (UGenericParticleSystemComponent*)UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		particleEffect,
		GetActorLocation(), FRotator(),
		true,
		EPSCPoolMethod::AutoRelease,
		true
	);
	
	if (particle == nullptr)
		return;

	particle->setLifeSpan(effectDuration);
}

void AGrenade::SetVelocity(FVector velocity)
{
	mesh->SetPhysicsLinearVelocity(velocity);
}