#include "QuietShoes.h"
#include "Base3C.h"
#include "Equipement.h"

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

}

void UQuietShoes::MUlToggleVisibility_Implementation(bool visible)
{
	if (visible)
		pawn->equipement->EquipDefault();
};


void UQuietShoes::PlayerPossess_Implementation()
{
	isActive = true;
}