#include "Terminal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "HelperClass.h"
#include <Officer.h>
#include "SecurityCamera.h"
#include <Kismet/GameplayStatics.h>

ATerminal::ATerminal()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->SetGenerateOverlapEvents(true);


	mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	mesh->SetupAttachment(Trigger);
}

void ATerminal::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATerminal::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ATerminal::OnTriggerOverlapEnd);
}

void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateProgressHack(DeltaTime);
}

void ATerminal::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    UTerminalUI* widget = nullptr;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        widget = Cast<UTerminalUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            thief->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateOfficer(TerminalHacked, TerminalOpenState));
        }
    }
}

void ATerminal::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ATerminal::Interact_Implementation(AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (TerminalHacked)
        {
            float realtimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

            float diff = realtimeSeconds - time;
            if (diff < 0.10f && time != 0)
                return;

            time = realtimeSeconds;

            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());

            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, !TerminalOpenState));

            playerController->SRToggleCameras(this, !TerminalOpenState);
        }
        else
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            widget->ActivateProgressBar();
        }

        return;
    }

    AOfficer* officer = Cast<AOfficer>(pActor);
    if (officer != nullptr)
    {
        if (TerminalHacked)
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());

            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateOfficer(TerminalHacked, !TerminalOpenState));
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

            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateOfficer(TerminalHacked, !TerminalOpenState));

            playerController->SRToggleCameras(this, !TerminalOpenState);
        }
    }
}

void ATerminal::StopInteract_Implementation(AActor* pActor)
{
    if (acteurUsingThis == nullptr)
        return;

    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<ABase3C>(pActor)->GetController());
    UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));

    if (Cast<AThief>(pActor) != nullptr)
    {
        widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
    }
    else
    {
        widget->SetDefaultText(widget->getTextStateOfficer(TerminalHacked, TerminalOpenState));
    }
}

void ATerminal::HackTerminal_Implementation(bool isHack)
{
    TerminalHacked = isHack;
    UpdateUIText();
}

void ATerminal::ToggleCameras_Implementation(bool pOpen)
{
    if (TerminalOpenState == pOpen)
        return;

    TerminalOpenState = pOpen;

    for (ASecurityCamera* camera : SecurityCameras)
    {
        if (camera == nullptr)
            continue;

        if (!pOpen)
            camera->frozen = true;
        else
            camera->frozen = false;
    }

    UpdateUIText();
}

void ATerminal::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        ABase3C* player = Cast<ABase3C>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        AOfficer* officer = Cast<AOfficer>(acteurUsingThis);
        if (officer != nullptr)
        {
            if (currentTime >= TimeToFixOfficer)
            {
                currentlyInteracting = false;
                currentTime = 0;

                widget->SetDefaultText(widget->getTextStateOfficer(false, TerminalOpenState));

                playerController->SRToogleHackTerminal(this, false);
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

                widget->SetDefaultText(widget->getTextStateThief(true, TerminalOpenState));

                playerController->SRToogleHackTerminal(this, true);
            }
            else
            {
                widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToHackThief, 0, 1));
            }
        }
    }
}

void ATerminal::UpdateUIText_Implementation()
{
    TArray<AActor*> actors;
    Trigger->GetOverlappingActors(actors, ABase3C::StaticClass());
    for (AActor* actor : actors)
    {
        ABase3C* player = Cast<ABase3C>(actor);

        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        if (playerController == nullptr)
            continue;

        UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            continue;

        if (Cast<AOfficer>(actor))
            widget->SetDefaultText(widget->getTextStateOfficer(TerminalHacked, TerminalOpenState));
        else
            widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
    }
}