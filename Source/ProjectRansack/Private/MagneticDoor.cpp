#include "MagneticDoor.h"
#include "Base3C.h"
#include <GamePlayerController.h>
#include "Thief.h"
#include "Officer.h"
#include "Components/ShapeComponent.h"
#include <Kismet/GameplayStatics.h>

AMagneticDoor::AMagneticDoor()
{
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
            widget->SetDefaultText(widget->getTextStateThief(thief->HasMagnetCard, FuseStateOpen));

            if (thief->HasMagnetCard && FuseStateOpen)
                StartInteract(thief);
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));

            if (FuseStateOpen)
                StartInteract(officer);
        }
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
            if (actor->IsA(AOfficer::StaticClass()))
                return;

            AThief* athief = Cast<AThief>(actor);
            if (athief != nullptr && athief->HasMagnetCard)
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

        if (Cast<AOfficer>(actor))
            widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));
        else
            widget->SetDefaultText(widget->getTextStateThief(Cast<AThief>(actor)->HasMagnetCard, FuseStateOpen));
    }
}