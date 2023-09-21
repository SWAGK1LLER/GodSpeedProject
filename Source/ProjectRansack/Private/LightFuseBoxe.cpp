#include "LightFuseBoxe.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "Engine/Light.h"
#include "HelperClass.h"
#include <Officer.h>
#include <Kismet/GameplayStatics.h>

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

    UpdateProgressHack(DeltaTime);
}

void ALightFuseBoxe::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* thief = Cast<AThief>(OtherActor);
    if (thief != nullptr)
    {
        thief->closeItems.Add(this);

        AController* PC = thief->GetController();
        if (PC != nullptr && PC->IsLocalPlayerController())
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

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
            ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));
        }

        return;
    }
}

void ALightFuseBoxe::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ALightFuseBoxe::Interact_Implementation(AActor* pActor)
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

            ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
            widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, !FuseStateOpen));

            playerController->SRToggleLights(this, !FuseStateOpen);
        }
        else
        {
            currentlyInteracting = true;
            currentTime = 0;
            acteurUsingThis = pActor;

            //Show progress bar
            AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
            ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
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

        ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateOfficer(!FuseStateOpen));

        playerController->SRToggleLights(this, !FuseStateOpen);
    }
}

void ALightFuseBoxe::StopInteract_Implementation(AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        if (acteurUsingThis == nullptr)
            return;

        currentlyInteracting = false;
        acteurUsingThis = nullptr;

        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
        widget->SetDefaultText(widget->getTextStateThief(FuseBoxHacked, FuseStateOpen));
    }
}

void ALightFuseBoxe::HackLights_Implementation()
{
    FuseBoxHacked = true;

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        FuseBoxHacked = false;
        UpdateUIText();
    }), HackDuration, false);
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

    UpdateUIText();
}

void ALightFuseBoxe::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
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

            ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
            if (widget == nullptr)
                return;

            widget->SetDefaultText(widget->getTextStateThief(true, FuseStateOpen));

            playerController->SRHackLights(this);
        }
    }
}

void ALightFuseBoxe::UpdateUIText_Implementation()
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
                ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
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
                ULightFuseBoxUI* widget = Cast<ULightFuseBoxUI>(playerController->GetWidget(this));
                if (widget == nullptr)
                    return;

                widget->SetDefaultText(widget->getTextStateOfficer(FuseStateOpen));
                return;
            }
        }
    }
}