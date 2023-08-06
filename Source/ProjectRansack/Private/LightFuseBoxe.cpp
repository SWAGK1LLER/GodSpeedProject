#include "LightFuseBoxe.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "Engine/Light.h"
#include "HelperClass.h"

ALightFuseBoxe::ALightFuseBoxe()
{
 	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->SetGenerateOverlapEvents(true);
	

    mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
    mesh->SetupAttachment(Trigger);
}

void ALightFuseBoxe::BeginPlay()
{
	Super::BeginPlay();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ALightFuseBoxe::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ALightFuseBoxe::OnTriggerOverlapEnd);
}

void ALightFuseBoxe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
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

            //HackLights();

            AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
            playerController->SRToggleLights(this, false);

            UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
            if (widget == nullptr)
                return;

            widget->ActivateDefaultText();
        }
    }
}

void ALightFuseBoxe::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    player->closeItems.Add(this);

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->AddInteractibleWidgetUI(this, WidgetClass);
    }
}

void ALightFuseBoxe::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    player->closeItems.Remove(this);

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->RemoveInteractibleWidgetUI(this);
    }
}

void ALightFuseBoxe::Interact_Implementation(AActor* pActor)
{
    /*AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr && FuseBoxHacked)
    {
        ToggleLights(!FuseStateOpen);
    }*/

    if (!FuseStateOpen)
        return;

    currentlyInteracting = true;
    currentTime = 0;
    acteurUsingThis = pActor;

    //Show progress bar
    AThief* thief = Cast<AThief>(pActor);
    AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
    UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
    widget->ActivateProgressBar();
}

void ALightFuseBoxe::StopInteract_Implementation(AActor* pActor)
{
    if (acteurUsingThis == nullptr)
        return;

    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    //Show basic message
    AThief* thief = Cast<AThief>(pActor);
    AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
    UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
    if (widget == nullptr)
        return;

    widget->ActivateDefaultText();
}

void ALightFuseBoxe::HackLights_Implementation()
{
    FuseBoxHacked = true;
}

void ALightFuseBoxe::ToggleLights_Implementation(bool pOpen)
{
    if (FuseStateOpen == pOpen)
        return;

    FuseStateOpen = pOpen;


    for (ALight* light : ConnectedLight)
    {
        if (!pOpen)
            HelperClass::deactivateActor(light);
        else
            HelperClass::activateActor(light);
    }

    if (!pOpen)
    {
        FTimerHandle Handle;
        GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
            ToggleLights(true);
        }), LightOffDuration, false);
    }
}