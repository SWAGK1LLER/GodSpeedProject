#include "Item.h"
#include "Components/BoxComponent.h"
#include "../ThirdPlayerCharacter.h"

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

}

void AItem::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThirdPlayerCharacter* player = Cast<AThirdPlayerCharacter>(OtherActor);
    if (player == nullptr)
        return;

    player->AddItem(*this);
}

void AItem::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}