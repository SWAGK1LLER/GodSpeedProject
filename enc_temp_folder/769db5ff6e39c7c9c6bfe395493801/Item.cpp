#include "Item.h"
#include "Components/BoxComponent.h"
#include "Thief.h"

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

    Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;

    player->AddItem(*this);

    //AController* PC = player->GetController();
    //if (PC != nullptr && PC->IsLocalPlayerController())
    //    Widget->AddToViewport();
}

void AItem::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AThief* player = Cast<AThief>(OtherActor);
    if (player == nullptr)
        return;
    
    //AController* PC = player->GetController();
    //if (PC != nullptr && PC->IsLocalPlayerController())
    //    Widget->RemoveFromParent();
}