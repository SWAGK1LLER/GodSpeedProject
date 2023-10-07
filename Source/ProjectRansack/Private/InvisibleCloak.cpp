#include "InvisibleCloak.h"
#include "Base3C.h"

UInvisibleCloak::UInvisibleCloak()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInvisibleCloak::BeginPlay()
{
    Super::BeginPlay();
};

void UInvisibleCloak::MUlFire_Implementation()
{
    if (abilityUsed)
        return;

    abilityUsed = true;
    pawn->GetMesh()->SetVisibility(false);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        abilityUsed = false;
        pawn->GetMesh()->SetVisibility(true);
    }), duration, false);
}

void UInvisibleCloak::MUlToggleVisibility_Implementation(bool visible)
{
    isActive = visible;
}

void UInvisibleCloak::UpdateUI_Implementation()
{
    pawn->WidgetUI->ShowCloakEquiped();
}