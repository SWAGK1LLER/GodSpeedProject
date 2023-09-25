#include "SlidingDoor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "HelperClass.h"
#include <Officer.h>
#include <Kismet/GameplayStatics.h>

ASlidingDoor::ASlidingDoor()
{
}

void ASlidingDoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateProgressHack(DeltaTime);
}

void ASlidingDoor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    USlidingDoorUI* widget = nullptr;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        widget = Cast<USlidingDoorUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            thief->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, FuseStateOpen));

            if (!FuseBoxHacked && FuseStateOpen && !manullyOpen)
            {
                StartInteract(officer);
            }
        }
    }
}

void ASlidingDoor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
        {
            thief->closeItems.Remove(this);
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Remove(this);

            TArray<AActor*> actors;
            Trigger->GetOverlappingActors(actors, AOfficer::StaticClass());

            if (actors.Num() == 0 && !FuseBoxHacked && !FuseStateOpen && !manullyOpen)
            {
                StartInteract(officer, false);
            }
        }
    }
}

void ASlidingDoor::StopInteract_Implementation(AActor* pActor)
{
    if (acteurUsingThis == nullptr)
        return;

    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<ABase3C>(pActor)->GetController());
    USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));

    if (Cast<AThief>(pActor) != nullptr)
    {
        widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
    else
    {
        widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, FuseStateOpen));
    }
}

void ASlidingDoor::StartInteract(AActor* pActor, bool pManullyOpen)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (FuseBoxHacked)
        {
            float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

            float diff = realtimeSeconds - time;
            if (diff < 0.10f && time != 0)
                return;

            time = realtimeSeconds;

            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());

            USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, !FuseStateOpen));

            playerController->SRToggleDoor(this, !FuseStateOpen, false);
        }
        else
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
            USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
            widget->ActivateProgressBar();
        }

        return;
    }

    AOfficer* officer = Cast<AOfficer>(pActor);
    if (officer != nullptr)
    {
        if (FuseBoxHacked)
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());

            USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, !FuseStateOpen));
            widget->ActivateProgressBar();
        }
        else
        {
            float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

            float diff = realtimeSeconds - time;
            if (diff < 0.10f && time != 0)
                return;

            time = realtimeSeconds;

            AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());

            USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
            if (widget != nullptr)
                widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, !FuseStateOpen));

            playerController->SRToggleDoor(this, !FuseStateOpen, pManullyOpen);
        }
    }
}

void ASlidingDoor::ToogleHackDoor_Implementation(bool isHack)
{
    FuseBoxHacked = isHack;
    UpdateUIText();
}

void ASlidingDoor::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        ABase3C* player = Cast<ABase3C>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        AOfficer* officer = Cast<AOfficer>(acteurUsingThis);
        if (officer != nullptr)
        {
            if (currentTime >= TimeToFixOfficer)
            {
                currentlyInteracting = false;
                currentTime = 0;

                widget->SetDefaultText(widget->getTextStateOfficer(false, FuseStateOpen));

                playerController->SRToogleHackDoor(this, false);
            }
            else
            {
                widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToFixOfficer, 0, 1));
            }
        }
        else
        {
            if (currentTime >= TimeToHackThief)
            {
                currentlyInteracting = false;
                currentTime = 0;

                widget->SetDefaultText(widget->getTextStateThief(true, FuseStateOpen));

                playerController->SRToogleHackDoor(this, true);
            }
            else
            {
                widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToHackThief, 0, 1));
            }
        }
    }
}

void ASlidingDoor::UpdateUIText_Implementation()
{
    TArray<AActor*> actors;
    Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());
    for (AActor* actor : actors)
    {
        ABase3C* player = Cast<ABase3C>(actor);

        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        if (playerController == nullptr)
            continue;

        USlidingDoorUI* widget = Cast<USlidingDoorUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            continue;

        if (Cast<AOfficer>(actor))
            widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, FuseStateOpen));
        else
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
}
