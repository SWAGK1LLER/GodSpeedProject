#include "EMP.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Base3C.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.h"

UEMP::UEMP()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEMP::BeginPlay()
{
	Super::BeginPlay();
}

void UEMP::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isActive)
		timer -= DeltaTime;
}

void UEMP::MUlFire_Implementation()
{
    if (timer > 0)
        return;

	timer = cooldown;

	CheckHit();

	if (controller == nullptr/* || !controller->IsLocalPlayerController()*/)
		return;
	
	controller->SRSpawnParticle(particleEffect, pawn->GetActorTransform(), -1);
}

void UEMP::MUlToggleVisibility_Implementation(bool visible)
{
    isActive = visible;

	if (!visible && timer > 0)
		timer = cooldown;
}

void UEMP::UpdateUI_Implementation()
{
    pawn->WidgetUI->ShowEmpEquiped();
}

void UEMP::CheckHit()
{
	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// start and end locations
	FVector SweepStart = pawn->GetActorLocation();
	FVector SweepEnd = pawn->GetActorLocation();

	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(Radius);

	//DrawDebugSphere(GetWorld(), SweepStart, MyColSphere.GetSphereRadius(), 50, FColor::Purple, true);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldStatic, MyColSphere);

	if (isHit)
	{
		for (FHitResult& Hit : OutHits)
		{
			ABase3C* player = Cast<ABase3C>(Hit.GetActor());
			if (player == nullptr || player == pawn)
				continue;

			//FreezePlayer
			APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			Cast<AGamePlayerController>(pc)->SRFreezeInput(StunDuration, player, SweepStart);
		}
	}
}