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

	if (!move)
		return;

	FHitResult hit;
	FVector start = GetActorLocation();
	FVector end = start + (GetActorForwardVector() * 200);
	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel2, QueryParams);

	if (!hit.bBlockingHit)
		Move();
	else
		move = false;
}

void ADecoyActor::Move()
{
	AddMovementInput(GetActorForwardVector(), 1);
}

void ADecoyActor::SetMesh_Implementation(USkeletalMesh* originalMesh, UClass* originalAnim)
{
	GetMesh()->SetSkeletalMesh(originalMesh);
	GetMesh()->SetAnimInstanceClass(originalAnim);
}