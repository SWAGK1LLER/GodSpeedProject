#include "SonicSensibility.h"
#include "Base3C.h"
#include "Equipement.h"

USonicSensibility::USonicSensibility()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USonicSensibility::BeginPlay()
{
	Super::BeginPlay();
}

void USonicSensibility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USonicSensibility::UpdateUI_Implementation()
{
	
}

void USonicSensibility::MUlToggleVisibility_Implementation(bool visible)
{
	if (visible)
		pawn->equipement->EquipDefault();
};

void USonicSensibility::PlayerPossess_Implementation()
{
	isActive = true;
}