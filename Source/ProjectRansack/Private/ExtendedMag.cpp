#include "ExtendedMag.h"
#include "Base3C.h"
#include "Equipement.h"
#include "Gun.h"

UExtendedMag::UExtendedMag()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UExtendedMag::BeginPlay()
{
	Super::BeginPlay();
}

void UExtendedMag::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UExtendedMag::UpdateUI_Implementation()
{
	//pawn->WidgetUI->ShowDecoy();
}

void UExtendedMag::PlayerPossess_Implementation()
{
	pawn->equipement->StunWeapon->maxAmmo += amountAdded;
	pawn->equipement->StunWeapon->ammo = pawn->equipement->StunWeapon->maxAmmo;
}