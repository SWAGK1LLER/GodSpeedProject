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


    //Set Door animation
    FOnTimelineFloat TimelineProgress;
    FOnTimelineEventStatic onTimelineFinishedCallback;
    TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
    doorTimeLine.AddInterpFloat(doorAnimation, TimelineProgress);
    doorTimeLine.SetLooping(false);

    onTimelineFinishedCallback.BindUFunction(this, FName("TimelineFinished"));
    doorTimeLine.SetTimelineFinishedFunc(onTimelineFinishedCallback);

    startingLocation = mesh->GetComponentLocation();
}

void ADoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    doorTimeLine.TickTimeline(DeltaTime);
}

void ADoor::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ADoor::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ADoor::Interact_Implementation(AActor* pActor)
{
    StartInteract(pActor, true);
}

void ADoor::StartInteract(AActor* pActor, bool pManullyOpen)
{
}

void ADoor::TimelineProgress(float value)
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
}

void ADoor::TimelineFinished()
{
    animationRunning = false;
}

void ADoor::HandleDoorAnimation()
{
    if (animationRunning)
    {
        if (!FuseStateOpen)
            doorTimeLine.Play();
        else
            doorTimeLine.Reverse();
    }
    else
    {
        animationRunning = true;

        if (!FuseStateOpen)
            doorTimeLine.PlayFromStart();
        else
            doorTimeLine.ReverseFromEnd();
    }
}

void ADoor::ToggleDoor_Implementation(bool pOpen, bool pManully)
{
    if (FuseStateOpen == pOpen)
        return;

    if (!pOpen)
        manullyOpen = pManully;
    else
        manullyOpen = false;

    FuseStateOpen = pOpen;

    HandleDoorAnimation();

    UpdateUIText();
}

void ADoor::UpdateUIText_Implementation()
{
}
