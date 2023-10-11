#include "SonicSensibility.h"

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
	//pawn->WidgetUI->ShowDecoy();
}

void USonicSensibility::PlayerPossess_Implementation()
{
	isActive = true;
}