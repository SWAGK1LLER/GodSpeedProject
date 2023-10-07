#include "SmokeGrenade.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include "Base3C.h"
#include <Kismet/GameplayStatics.h>

void ASmokeGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (counterStarted || owner == nullptr || OtherActor == owner)
		return;

	counterStarted = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
		Explose();
	}), timeBeforeExploseOnContact, false);
}

void ASmokeGrenade::Explose()
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