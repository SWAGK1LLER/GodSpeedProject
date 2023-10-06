#include "InvisibleCloak.h"
#include "Base3C.h"

UInvisibleCloak::UInvisibleCloak()
{
    PrimaryComponentTick.bCanEverTick = false;
    owner = Cast<ABase3C>(GetOwner());
}

void UInvisibleCloak::BeginPlay()
{

}

void UInvisibleCloak::MUlFire_Implementation()
{
    if (isActive)
        return;

    isActive = true;
    owner->GetMesh()->SetVisibility(false);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] {
        isActive = false;
        owner->GetMesh()->SetVisibility(true);
    }), duration, false);
}

void UInvisibleCloak::MUlToggleVisibility_Implementation(bool visible)
{
}

void UInvisibleCloak::UpdateUI_Implementation()
{
    owner->WidgetUI->ShowCloakEquiped();
}