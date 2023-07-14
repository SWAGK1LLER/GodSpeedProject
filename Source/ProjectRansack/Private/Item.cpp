#include "Item.h"
#include "Components/BoxComponent.h"
#include "Thief.h"
#include <GamePlayerController.h>

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Root"));
    Trigger->SetGenerateOverlapEvents(true);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnTriggerOverlapBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AItem::OnTriggerOverlapEnd);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (currentlyInteracting)
    {
        currentTime += DeltaTime;
        
        //Update ui progress bar
        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UItemWidgetUI* widget = playerController->GetWidget(this);
        widget->setProgressBarValue(map(currentTime, 0, TimeToPickUp, 0, 1));
    }

    if (currentTime >= TimeToPickUp && currentlyInteracting)
    {
        AThief* player = Cast<AThief>(acteurUsingThis);
        player->AddItem(*this);

        AThief* thief = Cast<AThief>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        playerController->RemoveInteractibleWidgetUI(this);
        currentlyInteracting = false;
        acteurUsingThis = nullptr;

        //Tell to server no body can take it anymore
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
    UItemWidgetUI* widget = playerController->GetWidget(this);
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
    UItemWidgetUI* widget = playerController->GetWidget(this);
    widget->ActivateDefaultText();
}