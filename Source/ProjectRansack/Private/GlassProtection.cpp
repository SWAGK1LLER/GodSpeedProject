#include "GlassProtection.h"

void AGlassProtection::BeginPlay()
{
    Super::BeginPlay();

    //Set animation
    FOnTimelineFloat TimelineProgress;
    FOnTimelineEventStatic onTimelineFinishedCallback;
    TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
    GlassTimeLine.AddInterpFloat(GlassAnimation, TimelineProgress);
    GlassTimeLine.SetLooping(false);

    onTimelineFinishedCallback.BindUFunction(this, FName("TimelineFinished"));
    GlassTimeLine.SetTimelineFinishedFunc(onTimelineFinishedCallback);

    startingLocation = mesh->GetComponentLocation();
}

void AGlassProtection::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    GlassTimeLine.TickTimeline(DeltaTime);
}

void AGlassProtection::TimelineProgress(float value)
{
    FVector translationAnim;
    switch (slidingAxis)
    {
    case EGlassSlidingAxis::X: translationAnim = FVector(value, 0, 0);
        break;
    case EGlassSlidingAxis::Y: translationAnim = FVector(0, value, 0);
        break;
    case EGlassSlidingAxis::Z: translationAnim = FVector(0, 0, value);
        break;
    default:
        break;
    }

    mesh->SetWorldLocation(startingLocation + translationAnim);
}

void AGlassProtection::TimelineFinished()
{
    animationRunning = false;
}

void AGlassProtection::PlayAnimation()
{
    if (animationRunning)
        return;

    animationRunning = true;
    GlassTimeLine.PlayFromStart();
}