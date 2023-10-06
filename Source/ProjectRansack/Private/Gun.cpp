#include "Gun.h"
#include "GamePlayerController.h"
#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include "SecurityCamera.h"
#include "Base3C.h"
#include "PlayerUI.h"

UGun::UGun(const FObjectInitializer& ObjectInitializer) //: UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGun::BeginPlay()
{
	Super::BeginPlay();
	CoolDownCurrentTime = 0;
}

void UGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isReloading)
	{
		CoolDownCurrentTime -= DeltaTime;
		if (CoolDownCurrentTime < 0)
		{
			ammo = maxAmmo;
			isReloading = false;
		}
	}

	ABase3C* player = Cast<ABase3C>(GetOwner());
	if (player->WidgetUI == nullptr)
		return;

	if (CoolDownCurrentTime > 0)
		player->WidgetUI->UpdateCooldown(CoolDownCurrentTime);
	else
		player->WidgetUI->ShowGunReady();
}

void UGun::Reload()
{
	CoolDownCurrentTime = coolDown;
	ammo = 0;
	isReloading = true;
}

void UGun::MUlFire_Implementation()
{
	if (ammo <= 0)
		return;

	ammo--;
	if (ammo == 0)
	{
		CoolDownCurrentTime = coolDown;
		isReloading = true;
	}

	FVector hitLocation;
	AActor* actor = HitScan(hitLocation);
	if (actor == nullptr)
		return;

	bool hitableEnemy = CheckHittableActor(actor);

	FTransform position = FTransform(hitLocation);

	APawn* owner = Cast<APawn>(GetOwner());
	AGamePlayerController* playerController = Cast<AGamePlayerController>(owner->GetController());
	if (playerController == nullptr)
		return;

	if (!playerController->IsLocalPlayerController())
		return;

	playerController->SRSpawnParticle(particleEffect, position, (hitableEnemy ? StunDuration : -1));

	if (!hitableEnemy)
		return;

	HitEntity(playerController, actor);
}

AActor* UGun::HitScan(FVector& hitLocation)
{
	ABase3C* player = Cast<ABase3C>(GetOwner());

	FVector begin = player->cameraComponent->camera->K2_GetComponentLocation();

	FVector forward = UKismetMathLibrary::GetForwardVector(player->cameraComponent->camera->K2_GetComponentRotation());

	FVector LineTraceEnd = begin + (forward * Reach);

	FHitResult Hit;
	const FName TraceTag("HitTrace");
	FCollisionQueryParams TraceParams(TraceTag, false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		begin,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

	hitLocation = Hit.Location;

	return Hit.GetActor();
}

bool UGun::CheckHittableActor(AActor* pActorToCheck)
{
	for (TSubclassOf<AActor> Class : EnemyHittable)
		if (pActorToCheck->IsA(Class))
			return true;
	return false;
}

void UGun::HitEntity(AGamePlayerController* PlayerController, AActor* pActorToHit)
{
	if (Cast<ABase3C>(pActorToHit))
		PlayerController->SRFreezeInput(StunDuration, Cast<ABase3C>(pActorToHit), GetOwner());
	else if (Cast<ASecurityCamera>(pActorToHit))
		PlayerController->CameraFreezeInput(pActorToHit);
}

void UGun::UpdateUI_Implementation()
{
	Cast<ABase3C>(GetOwner())->WidgetUI->ShowGunEquipped();
}