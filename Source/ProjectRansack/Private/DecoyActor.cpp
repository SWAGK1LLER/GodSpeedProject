#include "DecoyActor.h"

ADecoyActor::ADecoyActor()
{
	PrimaryActorTick.bCanEverTick = true;
	QueryParams.AddIgnoredActor(this);
}

void ADecoyActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(lifeSpan);
}

void ADecoyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult hit;
	FVector start = GetActorLocation();
	FVector end = start + (GetActorForwardVector() * 200);
	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel2, QueryParams);

	if (hit.bBlockingHit)
		move = false;
	else
		Move();
}

void ADecoyActor::Move()
{
	AddMovementInput(GetActorForwardVector(), move ? 1 : 0);
}