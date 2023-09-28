#include "MagneticDoor.h"
#include "Base3C.h"
#include <GamePlayerController.h>
#include "Thief.h"
#include "Officer.h"
#include "Components/ShapeComponent.h"
#include <Kismet/GameplayStatics.h>

AMagneticDoor::AMagneticDoor()
{
    secondDoor = CreateDefaultSubobject<UStaticMeshComponent>(FName("Door2"));
    secondDoor->SetupAttachment(Trigger);
}

void AMagneticDoor::BeginPlay()
{
    Super::BeginPlay();

    startingLocationDoor2 = secondDoor->GetComponentLocation();
}

void AMagneticDoor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    UMagneticDoorUI* widget = nullptr;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        widget = Cast<UMagneticDoorUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            thief->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextState(thief->HasMagnetCard, FuseStateOpen));
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextState(officer->HasMagnetCard, FuseStateOpen));
        }

        if (Cast<ABase3C>(OtherActor)->HasMagnetCard && FuseStateOpen)
            StartInteract(OtherActor);
    }
}

void AMagneticDoor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->RemoveInteractibleWidgetUI(this);

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
            thief->closeItems.Remove(this);
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Remove(this);
        }

        TArray<AActor*> actors;
        Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());
        bool stillHaveSomeone = false;
        for (AActor* actor : actors)
        {
            ABase3C* base = Cast<ABase3C>(actor);
            if (base->HasMagnetCard)
                return;
        }

        if (!FuseStateOpen)
            StartInteract(OtherActor, false);
    }
}

void AMagneticDoor::StopInteract_Implementation(AActor* pActor)
{
}

void AMagneticDoor::StartInteract(AActor* pActor, bool pManullyOpen)
{ 
    if (pManullyOpen)
        return;

    float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

    float diff = realtimeSeconds - time;
    if (diff < 0.10f && time != 0)
        return;

    time = realtimeSeconds;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<ABase3C>(pActor)->GetController());
    playerController->SRToggleDoor(this, !FuseStateOpen, false);
}

void AMagneticDoor::UpdateUIText_Implementation()
{
    TArray<AActor*> actors;
    Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());
    for (AActor* actor : actors)
    {
        ABase3C* player = Cast<ABase3C>(actor);

        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        if (playerController == nullptr)
            continue;

        UMagneticDoorUI* widget = Cast<UMagneticDoorUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            continue;

        widget->SetDefaultText(widget->getTextState(player->HasMagnetCard, FuseStateOpen));
    }
}

void AMagneticDoor::TimelineProgress(float value)
{
    FVector translationAnim;
    switch (slidingAxis)
    {
    case ESlidingAxis::X: translationAnim = FVector(value, 0, 0);
        break;
    case ESlidingAxis::Y: translationAnim = FVector(0, value, 0);
        break;
    case ESlidingAxis::Z: translationAnim = FVector(0, 0, value);
        break;
    default:
        break;
    }

    mesh->SetWorldLocation(startingLocation + translationAnim);
    secondDoor->SetWorldLocation(startingLocationDoor2 - translationAnim);
}