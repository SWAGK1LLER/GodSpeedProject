#include "ExtractionZone.h"
#include "Components/BoxComponent.h"
#include "GamePlayerController.h"
#include "HelperClass.h"
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

    timeInZone += DeltaTime;

    AGamePlayerController* playerController = Cast<AGamePlayerController>(playerInZone->GetController());
    if (playerController == nullptr)
        return;

    UExtractionZoneUI* widget = Cast<UExtractionZoneUI>(playerController->GetWidget(this));
    widget->setProgressBarValue(HelperClass::mapValue(timeInZone, 0, timeToExtract, 0, 1));
}

void AExtractionZone::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    AController* PC = player->GetController();
    if (PC != nullptr && PC->IsLocalPlayerController())
    {
        AGamePlayerController* playerController = Cast<AGamePlayerController>(PC);
        playerController->AddInteractibleWidgetUI(this, WidgetClass);

        playerInZone = player;
        timeInZone = 0;
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
