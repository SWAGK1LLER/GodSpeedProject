#include "QuietShoes.h"

UQuietShoes::UQuietShoes()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuietShoes::BeginPlay()
{
	Super::BeginPlay();
}

void UQuietShoes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuietShoes::UpdateUI_Implementation()
{
	//pawn->WidgetUI->ShowDecoy();
}

void UQuietShoes::PlayerPossess_Implementation()
{
	isActive = true;
}