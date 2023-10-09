#include "Claymore.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Base3C.h"
#include <Kismet/GameplayStatics.h>
#include <GamePlayerController.h>

AClaymore::AClaymore()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));

	trigger = CreateDefaultSubobject<USphereComponent>(FName("Trigger"));
	trigger->SetGenerateOverlapEvents(true);
	trigger->AttachToComponent(mesh, FAttachmentTransformRules::KeepRelativeTransform);
}

void AClaymore::BeginPlay()
{
	Super::BeginPlay();
	
	trigger->OnComponentBeginOverlap.AddDynamic(this, &AClaymore::OnHitTriggerOverlapBegin);
}

void AClaymore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClaymore::OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABase3C* player = Cast<ABase3C>(OtherActor);
	if (player == nullptr || player == owner || owner == nullptr)
		return;

	if (owner->GetController() == nullptr/* || !controller->IsLocalPlayerController()*/)
		return;

	Cast<AGamePlayerController>(owner->GetController())->SRSpawnParticle(particleEffect, GetActorTransform(), -1);

	CheckHit();

	SetLifeSpan(0.1f);
	trigger->OnComponentBeginOverlap.RemoveAll(this);
}

void AClaymore::CheckHit()
{
	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// start and end locations
	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = GetActorLocation();

	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(trigger->GetUnscaledSphereRadius());

	//DrawDebugSphere(GetWorld(), SweepStart, MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldStatic, MyColSphere);

	if (isHit)
	{
		for (FHitResult& Hit : OutHits)
		{
			ABase3C* player = Cast<ABase3C>(Hit.GetActor());
			if (player == nullptr || player == owner)
				continue;

			//FreezePlayer
			APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			Cast<AGamePlayerController>(pc)->SRFreezeInput(StunDuration, player, SweepStart);
		}
	}
}