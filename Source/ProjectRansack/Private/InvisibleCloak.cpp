#include "InvisibleCloak.h"
#include "Base3C.h"
#include "GamePlayerController.h"

UInvisibleCloak::UInvisibleCloak()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInvisibleCloak::BeginPlay()
{
    Super::BeginPlay();
};

void UInvisibleCloak::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (timer > 0)
    {
        timer -= DeltaTime;

        if (timer < 0)
        {
            pawn->GetMesh()->SetVisibility(true);
            abilityUsed = false;
        }
    }
}

void UInvisibleCloak::MUlFire_Implementation()
{
    if (abilityUsed)
        return;

    abilityUsed = true;
    timer = duration;

    if (controller != nullptr)
    {
        controller->SetPawnVisibility(pawn, false);
    }

    //pawn->GetMesh()->SetVisibility(false);
}

void UInvisibleCloak::MUlToggleVisibility_Implementation(bool visible)
{
    isActive = visible;
}

void UInvisibleCloak::UpdateUI_Implementation()
{
    pawn->WidgetUI->ShowCloakEquiped();
}