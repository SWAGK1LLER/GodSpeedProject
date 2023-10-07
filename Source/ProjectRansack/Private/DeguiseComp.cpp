#include "DeguiseComp.h"
#include "Engine/SkeletalMesh.h"
#include "Base3C.h"
#include "Thief.h"
#include "Officer.h"

UDeguiseComp::UDeguiseComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeguiseComp::BeginPlay()
{
	Super::BeginPlay();
}

void UDeguiseComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDeguiseComp::finishAttachement()
{
    if (originalMesh == nullptr)
        originalMesh = pawn->GetMesh()->GetSkeletalMeshAsset();
}

void UDeguiseComp::MUlFire_Implementation()
{
    if (abilityUsed)
        return;

    abilityUsed = true;

    if (pawn->IsA(AThief::StaticClass()))
        pawn->GetMesh()->SetSkeletalMesh(officerMesh);
    else
        pawn->GetMesh()->SetSkeletalMesh(thiefMesh);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        abilityUsed = false;
        pawn->GetMesh()->SetSkeletalMesh(originalMesh);
    }), duration, false);
}

void UDeguiseComp::MUlToggleVisibility_Implementation(bool visible)
{
    isActive = visible;
}

void UDeguiseComp::UpdateUI_Implementation()
{
    pawn->WidgetUI->ShowDeguisementEquiped();
}