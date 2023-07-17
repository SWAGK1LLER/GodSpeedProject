#include "ExtractionZone.h"
#include "Components/BoxComponent.h"
#include "GamePlayerController.h"
#include "EOSGameInstance.h"
#include "HelperClass.h"
#include "Kismet/GameplayStatics.h"
#include "Thief.h"


AExtractionZone::AExtractionZone()
{
 	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Root"));
	Trigger->SetGenerateOverlapEvents(true);
}

void AExtractionZone::BeginPlay()
{
	Super::BeginPlay();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AExtractionZone::OnTriggerOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AExtractionZone::OnTriggerOverlapEnd);
}

void AExtractionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (playerInZone == nullptr)
        return;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(playerInZone->GetController());
    if (playerController == nullptr)
        return;

    UExtractionZoneUI* widget = Cast<UExtractionZoneUI>(playerController->GetWidget(this));

    if (playerInZone->inventory == nullptr || playerInZone->inventory->items.Num() == 0)
    {
        widget->ShowEmptyInventory();
        return;
    }

    timeInZone += DeltaTime;

    widget->ShowProgressBar();
    widget->setProgressBarValue(HelperClass::mapValue(timeInZone, 0, timeToExtract, 0, 1));

    if (timeInZone >= timeToExtract)
    {
        UEOSGameInstance* gameInstance = Cast<UEOSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

        playerInZone->SRClearItems(playerInZone->inventory->score, gameInstance->team);
        playerInZone->inventory->ClearInventory();
        timeInZone = 0;
    }
}

void AExtractionZone::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        playerInZone = player;
        timeInZone = 0;

        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        UExtractionZoneUI* widget = Cast<UExtractionZoneUI>(playerController->AddInteractibleWidgetUI(this, WidgetClass));


        if (playerInZone->inventory == nullptr || playerInZone->inventory->items.Num() == 0)
            widget->ShowEmptyInventory();
    }
}

void AExtractionZone::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->RemoveInteractibleWidgetUI(this);

        playerInZone = nullptr;
        timeInZone = 0;
    }
}
