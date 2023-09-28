#include "ProtectionLoot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Base3C.h"
#include "GamePlayerController.h"
#include "HelperClass.h"
#include "Thief.h"
#include "Item.h"

AProtectionLoot::AProtectionLoot()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
    Trigger->SetGenerateOverlapEvents(true);

    mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
    mesh->SetupAttachment(Trigger);
}

void AProtectionLoot::BeginPlay()
{
	Super::BeginPlay();

    for (AItem* item : itemToActivate)
        HelperClass::deactivateTrigger(item->Trigger);
	
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AProtectionLoot::OnTriggerOverlapBegin);
    Trigger->OnComponentEndOverlap.AddDynamic(this, &AProtectionLoot::OnTriggerOverlapEnd);
}

void AProtectionLoot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UpdateProgressHack(DeltaTime);
}

void AProtectionLoot::DisableSystem_Implementation()
{
    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    for (AItem* item : itemToActivate)
        HelperClass::activateTrigger(item->Trigger);

    TArray<AActor*> thiefInRange;
    Trigger->GetOverlappingActors(thiefInRange, AThief::StaticClass());
    for (AActor* actor : thiefInRange)
    {
        OnTriggerOverlapEnd(nullptr, actor, nullptr, 0);
        Cast<AThief>(actor)->TryGeneratingOverlapEvent();
    }

    HelperClass::deactivateTrigger(Trigger);
    
    //Play animation
    PlayAnimation();
}

void AProtectionLoot::PlayAnimation()
{

}

void AProtectionLoot::UpdateProgressHack(float DeltaTime)
{
    if (currentlyInteracting)
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        ABase3C* player = Cast<ABase3C>(acteurUsingThis);
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        if (currentTime >= TimeToHackThief)
        {
            currentlyInteracting = false;
            currentTime = 0;

            widget->SetDefaultText(widget->getText());

            playerController->SRDisableSystem(this);
        }
        else
            widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToHackThief, 0, 1));
    }
}

void AProtectionLoot::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    UProtectionLootUI* widget = nullptr;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            widget = Cast<UProtectionLootUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            thief->closeItems.Add(this);
            widget->SetDefaultText(widget->getText());
        }
    }
}

void AProtectionLoot::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABase3C* player = Cast<ABase3C>(OtherActor);
    if (player == nullptr)
        return;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
            playerController->RemoveInteractibleWidgetUI(this);

            thief->closeItems.Remove(this);
        }
    }
}

void AProtectionLoot::Interact_Implementation(class AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr)
    {
        currentlyInteracting = true;
        currentTime = 0;
        acteurUsingThis = pActor;

        //Show progress bar
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        widget->ActivateProgressBar();
    }
}

void AProtectionLoot::StopInteract_Implementation(AActor* pActor)
{
    if (acteurUsingThis == nullptr)
        return;

    currentlyInteracting = false;
    acteurUsingThis = nullptr;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(Cast<ABase3C>(pActor)->GetController());
    UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));

    widget->SetDefaultText(widget->getText());
}