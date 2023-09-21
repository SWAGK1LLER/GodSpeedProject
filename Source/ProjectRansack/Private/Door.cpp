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
    AThief* thief = Cast<AThief>(OtherActor);
    if (thief != nullptr)
    {
        thief->closeItems.Add(this);

        AController* PC = thief->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            UDoorUI* widget = Cast<UDoorUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
        }

        return;
    }

    AOfficer* officer = Cast<AOfficer>(OtherActor);
    if (officer != nullptr)
    {
        officer->closeItems.Add(this);

        AController* PC = officer->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            UDoorUI* widget = Cast<UDoorUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));
        }

        return;
    }
}

void ADoor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AThief* thief = Cast<AThief>(OtherActor);
    if (thief != nullptr)
    {
        thief->closeItems.Remove(this);

        AController* PC = thief->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            playerController->RemoveInteractibleWidgetUI(this);
        }

        return;
    }

    AOfficer* officer = Cast<AOfficer>(OtherActor);
    if (officer != nullptr)
    {
        officer->closeItems.Remove(this);

        AController* PC = officer->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            playerController->RemoveInteractibleWidgetUI(this);
        }

        return;
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
        float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

        float diff = realtimeSeconds - time;
        if (diff < 0.10f && time != 0)
            return;

        time = realtimeSeconds;

        AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());

        UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateOfficer(!FuseStateOpen));

        playerController->SRToggleDoor(this, !FuseStateOpen);
    }
}

void ADoor::StopInteract_Implementation(AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (acteurUsingThis == nullptr)
            return;

        currentlyInteracting = false;
        acteurUsingThis = nullptr;

        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
}

void ADoor::HackDoor_Implementation()
{
    FuseBoxHacked = true;

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        FuseBoxHacked = false;
        UpdateUIText();
        }), HackDuration, false);
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
        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToInteract, 0, 1));
    }

    if (currentTime >= TimeToInteract && currentlyInteracting)
    {
        AThief* player = Cast<AThief>(acteurUsingThis);
        if (player != nullptr)
        {
            currentlyInteracting = false;
            currentTime = 0;

            AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());

            UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
            if (widget == nullptr)
                return;

            widget->SetDefaultText(widget->getTextStateThief(true, FuseStateOpen));

            playerController->SRHackDoor(this);
        }
    }
}

void ADoor::UpdateUIText_Implementation()
{
    TArray<AActor*> actors;
    Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());

    for (AActor* actor : actors)
    {
        AThief* thief = Cast<AThief>(actor);
        if (thief != nullptr)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
            if (playerController != nullptr)
            {
                UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
                if (widget == nullptr)
                    return;

                widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
                return;
            }
        }

        AOfficer* officer = Cast<AOfficer>(actor);
        if (officer != nullptr)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());
            if (playerController != nullptr)
            {
                UDoorUI* widget = Cast<UDoorUI>(playerController->GetWidget(this));
                if (widget == nullptr)
                    return;

                widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));
                return;
            }
        }
    }
}
