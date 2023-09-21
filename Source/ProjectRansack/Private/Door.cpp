#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "HelperClass.h"
#include <Officer.h>
#include <Kismet/GameplayStatics.h>

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
    Trigger->SetGenerateOverlapEvents(true);


    mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
    mesh->SetupAttachment(Trigger);
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnTriggerOverlapBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnTriggerOverlapEnd);
}

void ADoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateProgressHack(DeltaTime);
}

void ADoor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    UDoorUI* widget = nullptr;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        widget = Cast<UDoorUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

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
        }
    }
}

void ADoor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
        }
    }
}

void ADoor::Interact_Implementation(AActor* pActor)
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

            UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, !FuseStateOpen));

            playerController->SRToggleDoor(this, !FuseStateOpen);
        }
        else
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
            UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
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

            UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
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

            UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, !FuseStateOpen));

            playerController->SRToggleDoor(this, !FuseStateOpen);
        }
    }
}

void ADoor::StopInteract_Implementation(AActor* pActor)
{
    if (acteurUsingThis == nullptr)
        return;

    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<ABase3C>(pActor)->GetController());
    UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));

    if (Cast<AThief>(pActor) != nullptr)
    {
        widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
    else
    {
        widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, FuseStateOpen));
    }
}

void ADoor::ToogleHackDoor_Implementation(bool isHack)
{
    FuseBoxHacked = isHack;
    UpdateUIText();
}

void ADoor::ToggleDoor_Implementation(bool pOpen)
{
    if (FuseStateOpen == pOpen)
        return;

    FuseStateOpen = pOpen;

    //TODO - waiting for door animation open/close or visual effect needed
    //Temporary action
    mesh->SetVisibility(pOpen);


    UpdateUIText();
}

void ADoor::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        ABase3C* player = Cast<ABase3C>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
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

void ADoor::UpdateUIText_Implementation()
{
    TArray<AActor*> actors;
    Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());
    for (AActor* actor : actors)
    {
        ABase3C* player = Cast<ABase3C>(actor);

        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        if (playerController == nullptr)
            continue;

        UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            continue;

        if (Cast<AOfficer>(actor))
            widget->SetDefaultText(widget->getTextStateOfficer(FuseBoxHacked, FuseStateOpen));
        else
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
}
