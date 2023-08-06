// Fill out your copyright notice in the Description page of Project Settings.


#include "StunWeapon.h"
#include "GamePlayerController.h"
#include "CameraComp.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericParticleSystemComponent.h"
#include "SecurityCamera.h"

UStunWeapon::UStunWeapon(const FObjectInitializer& ObjectInitializer) : UWeapon(ObjectInitializer)
{

}

void UStunWeapon::BeginPlay()
{
    Super::BeginPlay();
}


void UStunWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!maxAmmoSetUI)
    {
        ABase3C* player = Cast<ABase3C>(GetOwner());

        if (player->WidgetUI == nullptr)
            return;

        player->WidgetUI->UpdateRemainingAmmo(MaxAmmo);
        player->WidgetUI->SetMaxAmmo(MaxAmmo);
        maxAmmoSetUI = true;
    }
}

void UStunWeapon::Fire()
{
	if (CoolDownCurrentTime < coolDown || ammo - 1 < 0)
		return;

    CoolDownCurrentTime = 0;
	ammo--;

    ABase3C* player = Cast<ABase3C>(GetOwner());

    if(player->WidgetUI)
        player->WidgetUI->UpdateRemainingAmmo(ammo);

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


    playerController->SRSpawnParticle(particleEffect, position, (hitableEnemy ? StunDuration : -1) );

    if (!hitableEnemy)
        return;


    HitEntity(playerController, actor);


    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Actor hitted!"));
}

AActor* UStunWeapon::HitScan(FVector& hitLocation)
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

bool UStunWeapon::CheckHittableActor(AActor* pActorToCheck)
{
    for (TSubclassOf<AActor> Class : EnemyHittable)
    {
        if (pActorToCheck->IsA(Class))
            return true;
    }
    return false;
}

void UStunWeapon::HitEntity(AGamePlayerController* PlayerController, AActor* pActorToHit)
{
    if (Cast<ABase3C>(pActorToHit))
        PlayerController->SRFreezeInput(StunDuration, Cast<ABase3C>(pActorToHit));
    else if(Cast<ASecurityCamera>(pActorToHit))
    {
        PlayerController->CameraFreezeInput(pActorToHit);
    }
}
