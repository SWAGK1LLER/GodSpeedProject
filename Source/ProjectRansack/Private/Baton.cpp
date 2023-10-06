#include "Baton.h"
#include "Components/BoxComponent.h"
#include "GamePlayerController.h"
#include <HelperClass.h>
#include "Base3C.h"

UBaton::UBaton(const FObjectInitializer& ObjectInitializer) //: UStaticMeshComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	HitArea = CreateDefaultSubobject<UBoxComponent>(FName("HitArea"));
	HitArea->SetGenerateOverlapEvents(true);
	HitArea->SetupAttachment(this);
}

void UBaton::BeginPlay()
{
	Super::BeginPlay();

	HitArea->OnComponentBeginOverlap.AddDynamic(this, &UBaton::OnHitTriggerOverlapBegin);
	HitArea->OnComponentEndOverlap.AddDynamic(this, &UBaton::OnHitTriggerOverlapEnd);

	HelperClass::deactivateTrigger(HitArea);
	SetVisibility(false);
}

void UBaton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    CoolDownCurrentTime -= DeltaTime;
}

void UBaton::MUlFire_Implementation()
{
    if (isAttacking)
        return;

    //Play animation
    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<APawn>(GetOwner())->GetController());
    if (playerController == nullptr)
        return;

    playerController->MUlPlayAttackAnim(this);
    isAttacking = true;
}

void UBaton::OnHitTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player != nullptr)
    {
        if (CheckHittableActor(player))
            possibleHittedActor.Add(player);
    }
}

void UBaton::OnHitTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    /*ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player != nullptr)
        possibleHittedActor.Remove(player);*/
}

void UBaton::CheckPossibleHit()
{
    APawn* owner = Cast<APawn>(GetOwner());
    AGamePlayerController* playerController = Cast<AGamePlayerController>(owner->GetController());
    if (playerController == nullptr)
        return;

    for (ABase3C* player : possibleHittedActor)
        HitEntity(playerController, player);
}

void UBaton::MUlPlayAttack_Implementation()
{
    HelperClass::activateTrigger(HitArea);
    isAttacking = true;
}

void UBaton::deactivateTrigger()
{
    HelperClass::deactivateTrigger(HitArea);
    isAttacking = false;

    possibleHittedActor.Empty();
}

void UBaton::MUlToggleVisibility_Implementation(bool visible)
{
    SetVisibility(visible);
}

bool UBaton::CheckHittableActor(AActor* pActorToCheck)
{
    for (TSubclassOf<AActor> Class : EnemyHittable)
        if (pActorToCheck->IsA(Class))
            return true;
    return false;
}

void UBaton::HitEntity(AGamePlayerController* PlayerController, AActor* pActorToHit)
{
    if (Cast<ABase3C>(pActorToHit))
        PlayerController->SRFreezeInput(StunDuration, Cast<ABase3C>(pActorToHit), GetOwner()->GetActorLocation());
}

void UBaton::UpdateUI_Implementation()
{
    Cast<ABase3C>(GetOwner())->WidgetUI->ShowStunBattonEquiped();
}