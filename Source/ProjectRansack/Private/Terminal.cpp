// Fill out your copyright notice in the Description page of Project Settings.


#include "Terminal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "HelperClass.h"
#include <Officer.h>
// Sets default values
ATerminal::ATerminal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->SetGenerateOverlapEvents(true);


	mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	mesh->SetupAttachment(Trigger);
}

// Called when the game starts or when spawned
void ATerminal::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATerminal::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ATerminal::OnTriggerOverlapEnd);
}

// Called every frame
void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateProgressHack(DeltaTime);
}

void ATerminal::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* thief = Cast<AThief>(OtherActor);
    if (thief != nullptr)
    {
        thief->closeItems.Add(this);

        AController* PC = thief->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            UTerminalUI* widget = Cast<UTerminalUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
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
            UTerminalUI* widget = Cast<UTerminalUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            widget->SetDefaultText(widget->getTextStateOfficer(TerminalOpenState));
        }

        return;
    }
}

void ATerminal::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ATerminal::Interact_Implementation(AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (TerminalHacked)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());

            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, !TerminalOpenState));

            playerController->SRToggleCameras(this, !TerminalOpenState); //TODO
        }
        else
        {
            currentlyInteracting = true;
            currentTime = 0;
            UsedOwner = pActor;

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
        AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());

        UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateOfficer(!TerminalOpenState));

        playerController->SRToggleCameras(this, !TerminalOpenState); //TODO
    }
}

void ATerminal::StopInteract_Implementation(AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (UsedOwner == nullptr)
            return;

        currentlyInteracting = false;
        UsedOwner = nullptr;

        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
    }
}

void ATerminal::HackTerminal_Implementation()
{
    TerminalHacked = true;

    //FTimerHandle Handle;
    //GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
    //    TerminalHacked = false;
    //    UpdateUIText();
    //    }), HackDuration, false);
}

void ATerminal::DisableCameras_Implementation(bool pOpen)
{
    if (TerminalOpenState == pOpen)
        return;

    TerminalOpenState = pOpen;

   // mesh->SetVisibility(pOpen);


    UpdateUIText();
}

void ATerminal::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        AThief* thief = Cast<AThief>(UsedOwner);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToInteract, 0, 1));
    }

    if (currentTime >= TimeToInteract && currentlyInteracting)
    {
        AThief* player = Cast<AThief>(UsedOwner);
        if (player != nullptr)
        {
            currentlyInteracting = false;
            currentTime = 0;

            AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());

            UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
            if (widget == nullptr)
                return;

            widget->SetDefaultText(widget->getTextStateThief(true, TerminalOpenState));

            playerController->SRHackTerminal(this); //TODO
        }
    }
}

void ATerminal::UpdateUIText_Implementation()
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
                UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
                if (widget == nullptr)
                    return;

                widget->SetDefaultText(widget->getTextStateThief(TerminalHacked, TerminalOpenState));
                return;
            }
        }

        AOfficer* officer = Cast<AOfficer>(actor);
        if (officer != nullptr)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());
            if (playerController != nullptr)
            {
                UTerminalUI* widget = Cast<UTerminalUI>(playerController->GetWidget(this));
                if (widget == nullptr)
                    return;

                widget->SetDefaultText(widget->getTextStateOfficer(TerminalOpenState));
                return;
            }
        }
    }
}

