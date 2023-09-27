#include "StunStick.h"
#include "Components/BoxComponent.h"
#include "GamePlayerController.h"

UStunStick::UStunStick(const FObjectInitializer& ObjectInitializer) : UStunWeapon(ObjectInitializer)
{
    HitArea = CreateDefaultSubobject<UBoxComponent>(FName("HitArea"));
    HitArea->SetGenerateOverlapEvents(true);
    HitArea->SetupAttachment(this);
}

void UStunStick::BeginPlay()
{
    Super::BeginPlay();

    HitArea->OnComponentBeginOverlap.AddDynamic(this, &UStunStick::OnHitTriggerOverlapBegin);
    HitArea->OnComponentEndOverlap.AddDynamic(this, &UStunStick::OnHitTriggerOverlapEnd);
}

void UStunStick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStunStick::Fire()
{
    if (CoolDownCurrentTime > 0)
        return;

    CoolDownCurrentTime = coolDown;
    
    //Play animation
}

void UStunStick::OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player != nullptr)
    {
        if (CheckHittableActor(player))
            possibleHittedActor.Add(player);
    }
        
}

void UStunStick::OnHitTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player != nullptr)
        possibleHittedActor.Remove(player);
}

void UStunStick::CheckPossibleHit()
{
    APawn* owner = Cast<APawn>(GetOwner());
    AGamePlayerController* playerController = Cast<AGamePlayerController>(owner->GetController());
    if (playerController == nullptr)
        return;

    for (ABase3C* player : possibleHittedActor)
        HitEntity(playerController, player);
}