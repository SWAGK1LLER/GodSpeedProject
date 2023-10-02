#include "Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("mesh"));
	particleSys = CreateDefaultSubobject<UParticleSystemComponent>(FName("particule"));
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
	if (!counterStarted)
		return;

	counterStarted = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
		Explose();
	}), timeBeforeExploseOnContact, false);
}

void AGrenade::Explose()
{
	particleSys->ActivateSystem(true);

	SetLifeSpan(effectDuration);
}