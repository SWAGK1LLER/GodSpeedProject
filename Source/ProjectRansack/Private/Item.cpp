#include "Item.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>
#include "HelperClass.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Root"));
    Trigger->SetGenerateOverlapEvents(true);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnTriggerOverlapBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AItem::OnTriggerOverlapEnd);

    SpawningTransform = GetActorTransform();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (lootedItem || locked)
        return;

    if (currentlyInteracting)
    {
        currentTime += DeltaTime;
        
        //Update ui progress bar
        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToPickUp, 0, 1));
    }

    if (currentTime >= TimeToPickUp && currentlyInteracting)
    {
        locked = true;

        //Tell to server no body can take it anymore
        AThief* player = Cast<AThief>(acteurUsingThis);
        player->SRAddItem(this);
    }
}

void AItem::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AItem::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AItem::Interact_Implementation(AActor* pActor)
{
    currentlyInteracting = true;
    currentTime = 0;
    acteurUsingThis = pActor;

    //Show progress bar
    AThief* thief = Cast<AThief>(pActor);
    AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
    UItemWidgetUI* widget = Cast<UItemWidgetUI>(playerController->GetWidget(this));
    widget->ActivateProgressBar();
}

void AItem::StopInteract_Implementation(AActor* pActor)
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

void AItem::MulPlayerLootIt_Implementation()
{   
    HelperClass::deactivateActor(this);

    if (lootedItem)
        return;

    lootedItem = true;
    locked = true;
}

void AItem::dropItem(FVector dropLocation)
{
    HelperClass::activateActor(this);

    currentTime = TimeToPickUp;
    currentlyInteracting = false;
    lootedItem = false;
    locked = false;

    SetActorTransform(FTransform(dropLocation));

    if (Handle.IsValid())
        Handle.Invalidate();

    //Add 30 timer to pickup before respawn
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        
        if (lootedItem)
            return;

        reset();

    }), TimeBeforeRespawn, false);
}

void AItem::reset()
{
    HelperClass::activateActor(this);

    currentTime = 0;
    currentlyInteracting = false;
    lootedItem = false;
    locked = false;

    if (Handle.IsValid())
        Handle.Invalidate();

    SetActorTransform(SpawningTransform);
}