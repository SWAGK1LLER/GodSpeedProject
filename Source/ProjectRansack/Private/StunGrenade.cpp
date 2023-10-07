#include "StunGrenade.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Base3C.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.h"

AStunGrenade::AStunGrenade() : AGrenade()
{
}

void AStunGrenade::BeginPlay()
{
	Super::BeginPlay();
}

void AStunGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (exploded || owner == nullptr || OtherActor == owner)
		return;

	exploded = true;
	Explose();
}

void AStunGrenade::Explose()
{
	UGenericParticleSystemComponent* particle = (UGenericParticleSystemComponent*)UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		particleEffect,
		GetActorLocation(), FRotator(),
		true,
		EPSCPoolMethod::AutoRelease,
		true
	);

	SetLifeSpan(1);
	mesh->SetVisibility(false);
	CheckHit();
}

void AStunGrenade::CheckHit()
{
	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// start and end locations
	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = GetActorLocation();

	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(Radius);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldStatic, MyColSphere);

	if (isHit)
	{
		for (FHitResult& Hit : OutHits)
		{
			ABase3C* player = Cast<ABase3C>(Hit.GetActor());
			if (player == nullptr)
				continue;

			//FreezePlayer
			APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			Cast<AGamePlayerController>(pc)->SRFreezeInput(StunDuration, player, SweepStart);
		}
	}
}