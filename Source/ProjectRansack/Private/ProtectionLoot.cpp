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


    if (UsableByThief)
    {
        for (AItem* item : itemToActivate)
            HelperClass::activateTrigger(item->Trigger);

        TArray<AActor*> thiefInRange;
        Trigger->GetOverlappingActors(thiefInRange, AThief::StaticClass());
        for (AActor* actor : thiefInRange)
        {
            OnTriggerOverlapEnd(nullptr, actor, nullptr, 0);
            Cast<AThief>(actor)->TryGeneratingOverlapEvent();
        }

        //HelperClass::deactivateTrigger(Trigger);

        //Play animation
        PlayAnimation();
    }
    else
    {
        for (AItem* item : itemToActivate)
            HelperClass::deactivateTrigger(item->Trigger);

        TArray<AActor*> thiefInRange;
        Trigger->GetOverlappingActors(thiefInRange, AOfficer::StaticClass());
        for (AActor* actor : thiefInRange)
        {
            OnTriggerOverlapEnd(nullptr, actor, nullptr, 0);
            Cast<AOfficer>(actor)->TryGeneratingOverlapEvent();
        }

        PlayAnimation(true);
    }

    UsableByThief = !UsableByThief;
}

void AProtectionLoot::PlayAnimation(bool reverse)
{

}

void AProtectionLoot::UpdateProgressHack(float DeltaTime)
{
    ABase3C* player = Cast<ABase3C>(acteurUsingThis);
    if (currentlyInteracting && UsableByThief && player->IsA(AThief::StaticClass()))
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        if (currentTime >= TimeToHackThief)
        {
            currentlyInteracting = false;
            currentTime = 0;

            widget->SetDefaultText(widget->getTextStateThief());

            playerController->SRDisableSystem(this);
        }
        else
            widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToHackThief, 0, 1));
    }
    else if (currentlyInteracting && !UsableByThief && player->IsA(AOfficer::StaticClass()))
    {
        currentTime += DeltaTime;

        //Update ui progress bar
        AGamePlayerController* playerController = Cast<AGamePlayerController>(player->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        if (widget == nullptr)
            return;

        if (currentTime >= TimeToFixOfficer)
        {
            currentlyInteracting = false;
            currentTime = 0;

            widget->SetDefaultText(widget->getTextStateOfficer());

            playerController->SRDisableSystem(this);
        }
        else
            widget->setProgressBarValue(HelperClass::mapValue(currentTime, 0, TimeToFixOfficer, 0, 1));
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
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief && UsableByThief)
        {
            widget = Cast<UProtectionLootUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            thief->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateThief());
            return;
        }

        AOfficer* officer = Cast<AOfficer>(OtherActor);
        if (officer && !UsableByThief)
        {
            widget = Cast<UProtectionLootUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));

            officer->closeItems.Add(this);
            widget->SetDefaultText(widget->getTextStateOfficer());
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
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->RemoveInteractibleWidgetUI(this);

        AThief* thief = Cast<AThief>(OtherActor);
        if (thief)
        {
            thief->closeItems.Remove(this);
        }
        else
        {
            AOfficer* officer = Cast<AOfficer>(OtherActor);
            officer->closeItems.Remove(this);
        }
    }
}

void AProtectionLoot::Interact_Implementation(class AActor* pActor)
{
    AThief* thief = Cast<AThief>(pActor);
    if (thief != nullptr && UsableByThief)
    {
        currentlyInteracting = true;
        currentTime = 0;
        acteurUsingThis = pActor;

        //Show progress bar
        AGamePlayerController* playerController = Cast<AGamePlayerController>(thief->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        widget->ActivateProgressBar();
        return;
    }

    AOfficer* officer = Cast<AOfficer>(pActor);
    if (officer != nullptr && !UsableByThief)
    {
        currentlyInteracting = true;
        currentTime = 0;
        acteurUsingThis = pActor;

        //Show progress bar
        AGamePlayerController* playerController = Cast<AGamePlayerController>(officer->GetController());
        UProtectionLootUI* widget = Cast<UProtectionLootUI>(playerController->GetWidget(this));
        widget->ActivateProgressBar();
        return;
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

    if (Cast<AThief>(pActor) != nullptr)
    {
        widget->SetDefaultText(widget->getTextStateThief());
    }
    else
    {
        widget->SetDefaultText(widget->getTextStateOfficer());
    }
}